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

module Main where

import PBS.BSIParse
import PBS.BSIGenCC
import System.Environment
import System.Exit
import System.Console.GetOpt
import PBS.Utils

options =
  [ Option "o" ["output"] (fileArg formatCC) "output file"
  , Option "g" ["random-generator"] (fileArg genRandGen) "random generators file"
  , Option "t" ["tests"] (fileArg genTests) "tests file"
  ] where fileArg fn = ReqArg (\f -> writeFile f . fn) "<filename>"

usage = usageInfo "Usage: bsic [option...] input... \n  where option is:" options

type Action = AST -> IO ()

parseArgs :: [String] -> Either String ([Action], [String])
parseArgs args = do
  let (opts, inputs, errs) = getOpt Permute options args
  eassert (null errs) (concat errs)
  eassert (not $ null inputs) "No input given"
  return (opts, inputs)

main :: IO ()
main = parseArgs <$> getArgs >>= either fail run
  where fail msg = die (msg ++ "\n" ++ usage)

run :: ([Action], [String]) -> IO ()
run (actions, inputs) = do
  (diags, ast) <- parseBSIFiles inputs
  if null diags
    then mconcat actions ast
    else die $ unlines $ show <$> diags
