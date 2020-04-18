--TEST--
Bug #76439: Don't always strip leading whitespace from heredoc T_ENCAPSED_AND_WHITESPACE tokens
--FILE--
<?php

[$one, $two, $three, $four, $five, $six, $seven, $eight, $nine] = [1, 2, 3, 4, 5, 6, 7, 8, 9];

var_dump(<<<BAR
 $one-
 BAR);

var_dump(<<<BAR
 $two -
 BAR);

var_dump(<<<BAR
 $three	-
 BAR);

var_dump(<<<BAR
 $four-$four
 BAR);

var_dump(<<<BAR
 $five-$five-
 BAR);

var_dump(<<<BAR
 $six-$six-$six
 BAR);

var_dump(<<<BAR
 $seven
 -
 BAR);

var_dump(<<<BAR
 $eight
  -
 BAR);

var_dump(<<<BAR
$nine
BAR);

var_dump(<<<BAR
 -
 BAR);

var_dump(<<<BAR
  -
 BAR);

?>
--EXPECT--
string(2) "1-"
string(3) "2 -"
string(3) "3	-"
string(3) "4-4"
string(4) "5-5-"
string(5) "6-6-6"
string(3) "7
-"
string(4) "8
 -"
string(1) "9"
string(1) "-"
string(2) " -"
