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

module PBS.BSIParse
       ( Type(ScalarT, ListT, MapT)
       , Member(Member)
       , Decl(Struct, Enum, Tuple)
       , EnumValue(EnumValue)
       , AST
       , parseBSI
       , parseBSIFile
       , parseBSIFiles
       , nameof
       ) where

import Control.Monad
import Text.ParserCombinators.Parsec
import PBS.Utils
import qualified Data.Set as Set
import qualified Data.Map.Strict as Map
import Data.List

comment = string "//" >> skipMany (noneOf "\n")
__ = skipMany ((space >> return ()) <|> comment)

ident = liftM2 (:) identBegin (many identTail) <* __
identBegin = char '_' <|> letter
identTail = char '_' <|> alphaNum

int = (parseInt <$> many1 digit) <* __ where
  parseInt = read :: String -> Int

verbatim str = try $ string str <* __
keyword str = try $ string str <* notFollowedBy identTail <* __

lp = verbatim "("
rp = verbatim ")"
lb = verbatim "["
rb = verbatim "]"
lc = verbatim "{"
rc = verbatim "}"
arrow = verbatim "=>"
eqsign = verbatim "="
semi = verbatim ";"
comma = verbatim ","
colon = verbatim ":"
structkw = keyword "struct"
enumkw = keyword "enum"
tuplekw = keyword "tuple"

data Type = ScalarT String | ListT Type | MapT Type Type

typid = listType <|> mapType <|> scalarType
scalarType = ScalarT <$> ident
listType = lb *> (ListT <$> typid) <* rb
mapType = lp *> (liftM2 MapT typid (arrow >> typid)) <* rp

builtinTypes =
  ["string", "float", "double", "varuint"]
  ++ liftM2 (++) ["int", "uint"] ["8", "16", "32", "64"]

-- has tag if is tagged struct, nothing if tuple
data Member = Member Type String (Maybe Int)

member = liftM3 Member typid ident (eqsign >> Just <$> int) <* semi

tmember = liftM3 Member typid ident (return Nothing) <* semi

data Decl
  = Struct String [Member]
  | Enum String Type [EnumValue] -- name, underlying type, ...
  | Tuple String [Member]

nameof (Struct name _) = name
nameof (Enum name _ _) = name
nameof (Tuple name _)  = name

data EnumValue = EnumValue String (Maybe Int)

decl = struct <|> enum <|> tuple
struct = structkw >> liftM2 Struct ident (lc *> many member <* rc) <* semi
enum = enumkw >> liftM3 Enum ident (colon >> scalarType) (lc *> sepBy value comma <* rc) <* semi
  where value = liftM2 EnumValue ident (optionMaybe (eqsign >> int))
tuple = tuplekw >> liftM2 Tuple ident (lc *> many tmember <* rc) <* semi

decls = many decl

-- this is the topmost syntax we would like to parse, so skip leading whitespace
type AST = [Decl]
ast = __ *> decls <* eof

-- perform analysis after successful parse
parseBSI :: String -> String -> ([Diagnostics], AST)
parseBSI filename contents = runAnalyze $ parse ast filename contents

parseBSIFile :: String -> IO ([Diagnostics], AST)
parseBSIFile = fmap runAnalyze . parseFromFile ast

parseBSIFiles :: [String] -> IO ([Diagnostics], AST)
parseBSIFiles = fmap runAnalyze . foldMap (parseFromFile ast)

runAnalyze :: Either ParseError [Decl] -> ([Diagnostics], AST)
runAnalyze = either (\err -> ([ParseE err], mempty)) (\ast -> (analyze ast, ast))

-- Diagnostics wraps around ParseError from parsec, adding custom errors
data Diagnostics
  = ParseE ParseError
  | UnknownTypeE String String -- name of unknown type, and in which decl is it used
  | DuplicateDeclError String  -- name of dup decl
  | DuplicateTagError String Int [String] -- name of struct, tag, members using the tag
  | DuplicateMemberError String String    -- name of struct, duped member
  | TagOutOfRangeError String String Int  -- name of struct, member, tag

instance Show Diagnostics where
  show (ParseE e) = show e
  show (UnknownTypeE typname declname)
    = "Type "++typname++" is unknown when used in "++declname++"\n"
  show (DuplicateDeclError declname)
    = "Duplicate declaration for " ++ declname
  show (DuplicateTagError struct tag mems)
    = "In struct "++struct++", tag "++show tag++" is used by "++(show $ length mems)++" members: "
      ++ intercalate ", " mems
  show (DuplicateMemberError struct mem)
    = "Member name "++mem++" appeared more than once in struct "++struct
  show (TagOutOfRangeError struct mem tag)
    = "Tag "++show tag++" used by "++struct++"::"++mem++" is out of valid range [1, 255]"

-- analysis

type Scope = Set.Set String     -- currently names only

addToScope :: Decl -> Scope -> Scope
addToScope decl = Set.insert (nameof decl)

analyze :: [Decl] -> [Diagnostics]
analyze decls =
  let laScope = Set.fromList $ builtinTypes ++ (nameof <$> decls)
      initVisScope = Set.fromList builtinTypes
  in
    checkDecls laScope initVisScope decls

checkDecls :: Scope -> Scope -> [Decl] -> [Diagnostics]
checkDecls _ _ [] = []
checkDecls laScope visScope (decl:rest) =
  checkDecl laScope visScope decl
  ++ checkDecls laScope (addToScope decl visScope) rest

checkDecl :: Scope -> Scope -> Decl -> [Diagnostics]
checkDecl laScope visScope =
  mconcat [ checkDirectRefVisible visScope
          , checkDupDecl visScope
          , checkIdRefKnown laScope
          , checkTagRange
          , checkTagUnique
          , checkMemberNameUnique
          ]

checkDirectRefVisible visScope decl = case decl of
  Enum name (ScalarT typ) _ | Set.notMember typ visScope -> [UnknownTypeE typ name]
  Struct name mems -> [UnknownTypeE typ name
                      | Member (ScalarT typ) _ _ <- mems,
                        Set.notMember typ visScope]
  Tuple name mems -> checkDirectRefVisible visScope (Struct name mems)
  _ -> []

checkDupDecl visScope decl =
  let name = nameof decl in
  if Set.member name visScope then [DuplicateDeclError name] else []

-- indirect refs, ignoring enums
-- but checking direct refs in structs a second time because code will be easier
checkIdRefKnown laScope (Struct name mems) =
  let srefs = srefsOfT =<< [typ | Member typ _ _ <- mems]
        where srefsOfT (ScalarT t) = [t]
              srefsOfT (ListT t) = srefsOfT t
              srefsOfT (MapT a b) = srefsOfT a ++ srefsOfT b
  in
    [UnknownTypeE ref name | ref <- srefs, Set.notMember ref laScope]
checkIdRefKnown laScope (Tuple name mems) =
  checkIdRefKnown laScope (Struct name mems)
checkIdRefKnown _ _ = []

checkTagRange (Struct struct mems) =
  let oor = [(tag, mem) | Member _ mem (Just tag) <- mems, not (0<tag && tag<256)] in
  [TagOutOfRangeError struct mem tag | (tag, mem) <- oor]
checkTagRange _ = []

checkTagUnique (Struct struct mems) =
  let
    ms = [(tag, mem) | Member _ mem (Just tag) <- mems]
    dups = Map.filter (longerThan 1) (groupToMap ms)
  in
    uncurry (DuplicateTagError struct) <$> Map.assocs dups
checkTagUnique _ = []

checkMemberNameUnique (Struct struct mems) =
  let
    counts = countToMap [name | Member _ name _ <- mems]
    dups = Map.filter (>1) counts
  in
    DuplicateMemberError struct <$> Map.keys dups
checkMemberNameUnique _ = []
