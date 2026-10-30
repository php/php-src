--TEST--
var_dump(ReflectionConstant)
--EXTENSIONS--
zend_test
--FILE--
<?php

define('RT_CONST', 42);
const CT_CONST = [
    'foo' => 'foo',
    'bar' => ['bar'],
];

function dump($r) {
    var_dump($r);
    echo $r;
}

dump(new ReflectionConstant('ZEND_CONSTANT_A'));
dump(new ReflectionConstant('ZEND_TEST_DEPRECATED'));
dump(new ReflectionConstant('RT_CONST'));
dump(new ReflectionConstant('CT_CONST'));

?>
--EXPECT--
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(15) "ZEND_CONSTANT_A"
}
Constant [ <persistent> string ZEND_CONSTANT_A ] { global }
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(20) "ZEND_TEST_DEPRECATED"
}
Constant [ <persistent, deprecated> int ZEND_TEST_DEPRECATED ] { 42 }
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(8) "RT_CONST"
}
Constant [ int RT_CONST ] { 42 }
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(8) "CT_CONST"
}
Constant [ array CT_CONST ] { Array }
