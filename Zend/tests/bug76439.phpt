--TEST--
Bug #76439: Don't always strip leading whitespace from heredoc T_ENCAPSED_AND_WHITESPACE tokens
--FILE--
<?php

$foo = 1;

var_dump(<<<BAR
 $foo-
 BAR);

var_dump(<<<BAR
 $foo -
 BAR);

var_dump(<<<BAR
 $foo	-
 BAR);

var_dump(<<<BAR
 $foo-$foo
 BAR);

var_dump(<<<BAR
 $foo-$foo-
 BAR);

var_dump(<<<BAR
 $foo-$foo-$foo
 BAR);

var_dump(<<<BAR
 $foo
 -
 BAR);

var_dump(<<<BAR
 $foo
  -
 BAR);

var_dump(<<<BAR
$foo
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
string(3) "1 -"
string(3) "1	-"
string(3) "1-1"
string(4) "1-1-"
string(5) "1-1-1"
string(3) "1
-"
string(4) "1
 -"
string(1) "1"
string(1) "-"
string(2) " -"
