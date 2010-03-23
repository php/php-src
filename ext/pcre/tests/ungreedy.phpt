--TEST--
U (PCRE_UNGREEDY) modififer
--FILE--
<?php

var_dump(preg_match('/<.*>/', '<aa> <bb> <cc>', $m));
var_dump($m);

var_dump(preg_match('/<.*>/U', '<aa> <bb> <cc>', $m));
var_dump($m);

var_dump(preg_match('/(?U)<.*>/', '<aa> <bb> <cc>', $m));
var_dump($m);

?>
--EXPECT--
int(1)
array(1) {
  [0]=>
  string(14) "<aa> <bb> <cc>"
}
int(1)
array(1) {
  [0]=>
  string(4) "<aa>"
}
int(1)
array(1) {
  [0]=>
  string(4) "<aa>"
}
