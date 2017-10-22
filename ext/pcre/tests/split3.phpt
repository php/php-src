--TEST--
preg_split() - parenthesized expressions advance the limit count 
--FILE--
<?php

var_dump(
	preg_split("/(.)/", "HAHAHA, VERY FUNNY", 2, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE),
	preg_split("/(.)/", "HAHAHA, VERY FUNNY", 3, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE),
	preg_split("/(.)/", "HAHAHA, VERY FUNNY", null, PREG_SPLIT_NO_EMPTY | PREG_SPLIT_DELIM_CAPTURE)
);

--EXPECT---
array(2) {
  [0]=>
  string(1) "H"
  [1]=>
  string(17) "AHAHA, VERY FUNNY"
}
array(3) {
  [0]=>
  string(1) "H"
  [1]=>
  string(1) "A"
  [2]=>
  string(16) "HAHA, VERY FUNNY"
}
array(18) {
  [0]=>
  string(1) "H"
  [1]=>
  string(1) "A"
  [2]=>
  string(1) "H"
  [3]=>
  string(1) "A"
  [4]=>
  string(1) "H"
  [5]=>
  string(1) "A"
  [6]=>
  string(1) ","
  [7]=>
  string(1) " "
  [8]=>
  string(1) "V"
  [9]=>
  string(1) "E"
  [10]=>
  string(1) "R"
  [11]=>
  string(1) "Y"
  [12]=>
  string(1) " "
  [13]=>
  string(1) "F"
  [14]=>
  string(1) "U"
  [15]=>
  string(1) "N"
  [16]=>
  string(1) "N"
  [17]=>
  string(1) "Y"
}
