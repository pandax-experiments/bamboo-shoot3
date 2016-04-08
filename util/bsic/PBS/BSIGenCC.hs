--
--  Copyright 2016 Carl Lei
--
--  This file is part of Bamboo Shoot 3.
--
--  Bamboo Shoot 3 is free software: you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation, either version 3 of the License, or
--  (at your option) any later version.
--
--  Bamboo Shoot 3 is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with Bamboo Shoot 3.  If not, see <http://www.gnu.org/licenses/>.
--
--  Carl Lei <xecycle@gmail.com>

{-# LANGUAGE TemplateHaskell #-}

module PBS.BSIGenCC
       ( formatCC
       , genRandGen
       , genTests
       ) where

import PBS.BSIParse
import Control.Monad
import Data.List
import qualified Data.Map.Strict as Map
import Data.Maybe
import Data.FileEmbed
import Data.ByteString.Char8 (unpack)

formatType (ScalarT name) = transType name
formatType (ListT typ) = "std::vector<" ++ formatType typ ++ ">"
formatType (MapT l r) = "std::map<" ++ formatType l ++ ", " ++ formatType r ++ ">"

formatMember (Member typ name _) = formatType typ ++ " " ++ name ++ ";"

uncommas = intercalate ", "

formatDecl (Struct name members) =
    "struct " ++ name ++ " {\n"
    ++ unlines (formatMember <$> members)
    ++ "\n"
    ++ genEq name members
    ++ "\n\n"
    ++ formatTags name members ++ "\n"
    ++ "};"

formatDecl (Enum name typ values) =
  "enum class " ++ name ++ " : " ++ formatType typ ++ " {\n"
  ++ uncommas (formatValue <$> values) ++ "\n"
  ++ "};"
  where formatValue (EnumValue name Nothing) = name
        formatValue (EnumValue name (Just v)) = name ++ "=" ++ show v

formatDecl (Tuple name members) =
  "struct "++name++" {\n"
  ++ unlines (formatMember <$> members)
  ++ "\n"
  ++ genEq name members
  ++ "\n\n"
  ++ genCmp name members
  ++ "\n\n"
  ++ formatTupleTypedef name members ++ "\n"
  ++ "};"

formatDecl _ = ""

genEq structName [] =
  "bool operator==(const "++structName++"&) const\n{\n"
  ++ "return true;\n"
  ++ "}"

genEq structName ms =
  "bool operator==(const "++structName++"& other) const\n{\n"
  ++ "return " ++ unand (memeq <$> ms) ++ ";\n"
  ++ "}"
  where unand = intercalate " && "
        memeq (Member _ name _) = "(" ++ name ++ "==other." ++ name ++ ")"

genCmp structName [] =
  "bool operator<(const "++structName++"&) const\n{\n"
  ++ "return false;\n"
  ++ "}"

genCmp structName ms =
  "bool operator<(const "++structName++"& other) const\n{\n"
  ++ unlines (memlt <$> ms)
  ++ "return false;\n"
  ++ "}"
  where memlt (Member _ name _) =
          "if ("++name++" != other."++name++") return "++name++" < other."++name++";"

formatTags :: String -> [Member] -> String
formatTags structName ms =
  "PBSS_TAGGED_STRUCT(" ++ uncommas (formatTag structName <$> ms) ++ ");"
formatTag structName (Member _ name (Just tag)) =
  "PBSS_TAG_MEMBER(" ++ (show tag) ++ ", " ++ "&" ++ structName ++ "::" ++ name ++ ")"

formatTupleTypedef struct ms =
  "PBSS_TUPLE_MEMBERS(" ++ uncommas (formatDef struct <$> ms) ++ ");"
formatDef struct (Member _ name Nothing) =
  "PBSS_TUPLE_MEMBER(&"++struct++"::"++name++")"

genFwd decls = unlines $ filter (not . null) $ fwd <$> decls
  where fwd (Struct name _) = "struct "++name++";"
        fwd (Tuple name _)  = "struct "++name++";"
        fwd _ = ""

formatCC :: [Decl] -> String
formatCC decls =
  genFwd decls ++ "\n" ++
  intercalate "\n\n" (formatDecl <$> decls) ++
  "\n"

transType name = fromMaybe name (Map.lookup name translates)

translates = Map.fromList $
             [ ("string", "std::string")
             , ("varuint", "pbss::var_uint<std::size_t>")
             ] ++ ints
  where ints = (\t -> (t, (t++"_t"))) <$> liftM2 (++) ["int", "uint"] ["8", "16", "32", "64"]

genRandGen :: [Decl] -> String
-- the helpers file covered builtin types and enums, need to generate for
-- structs and tuples
genRandGen decls = unpack $(embedFile "random-helpers.cc") ++ "\n"
                   ++ intercalate "\n\n" (filter (not . null) $ randGen <$> decls) ++ "\n"

randGen (Struct name mems) =
  name ++ " inline bsic_rgen(bsic_rgen_tag<"++name ++ ">)\n{\n"
  ++"  return {"++ uncommas (memGen <$> mems) ++"};\n"
  ++"}"
  where memGen (Member typ _ _) = "bsic_rgen(bsic_rgen_tag<"++formatType typ++">{})"
randGen (Tuple name mems) = randGen (Struct name mems)
randGen _ = ""

genTests :: [Decl] -> String
genTests decls = unpack $(embedFile "test-helpers.cc") ++ "\n"
                 ++ "void bsic_run_generated_tests()\n{\n"
                 ++ unlines (genTest <$> decls)
                 ++ "}\n"

genTest decl = "  BSIC_TEST_TYPE("++nameof decl++");"
