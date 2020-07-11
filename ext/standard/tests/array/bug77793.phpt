--TEST--
Bug #77793: Segmentation fault in extract() when overwriting reference with itself
--FILE--
<?php

$str = 'foo';
$vars = ['var' => $str . 'bar'];
$var = &$vars['var'];
extract($vars);
var_dump($vars, $var);

?>
--EXPECT--
array(1) {
  ["var"]=>
  &string(6) "foobar"
}
string(6) "foobar"
