--TEST--
Bug #21730 (*scanf "%n" conversion flag gives string instead of integer)
--FILE--
<?php
$foo = "ABC = DEF";
$fmt = "%s = %s %n";
$res_a = array();

/* $res_a[2] is supposed to be a integer value that
 * represents the number of characters consumed so far
 */
sscanf($foo, $fmt, $res_a[0], $res_a[1], $res_a[2]);

$res_b = sscanf($foo, $fmt);

var_dump($res_a);
var_dump($res_b);
?>
--EXPECTS--
array(3) {
  [0]=>
  string(3) "ABC"
  [1]=>
  string(3) "DEF"
  [2]=>
  int(9)
}
array(3) {
  [0]=>
  string(3) "ABC"
  [1]=>
  string(3) "DEF"
  [2]=>
  int(9)
}
