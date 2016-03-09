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

module PBS.Utils
       ( eassert
       , longerThan
       , atmost
       , groupToMap
       , countToMap
       ) where

import qualified Data.Map.Strict as Map
import Control.Applicative

-- assert returning Either
eassert :: Bool -> e -> Either e ()
eassert good msg = if good then Right () else Left msg

-- length of foldable is longer than n
longerThan n = not . null . drop n

-- length not longer than n
atmost n = null . drop n

groupToMap :: (Ord k) => [(k, v)] -> Map.Map k [v]
groupToMap = foldr addToMultiMap Map.empty
  where
    addToMultiMap = \(k, v) -> Map.alter (Just . maybe [v] (v:)) k

countToMap :: (Ord a) => [a] -> Map.Map a Int
countToMap = foldl (flip incr) Map.empty
  where
    incr k = Map.alter (Just . maybe 1 (1+)) k

-- add Monoid instances for common Monads
instance (Monoid r) => Monoid (Either l r) where
  mempty = pure mempty
  mappend = liftA2 mappend

instance (Monoid a) => Monoid (IO a) where
  mempty = pure mempty
  mappend = liftA2 mappend
