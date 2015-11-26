--TEST--
Bug #54238 (use-after-free in substr_replace())
--INI--
error_reporting=E_ALL&~E_NOTICE
--FILE--
<?php
$f = array(array('A', 'A'));

$z = substr_replace($f, $f, $f, 1);
var_dump($z, $f);
?>
--EXPECT--
array(1) {
  [0]=>
  string(9) "AArrayray"
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    string(1) "A"
    [1]=>
    string(1) "A"
  }
}
