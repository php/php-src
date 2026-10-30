--TEST--
Backtrace args of a #[SensitiveParameter] variadic called with named arguments
--FILE--
<?php
class E extends Exception {}

function outer(#[SensitiveParameter] ...$args) {
    return new E();
}

var_dump(outer(a: 1, b: 'secret')->getTrace()[0]['args']);
?>
--EXPECT--
array(2) {
  ["a"]=>
  object(SensitiveParameterValue)#2 (0) {
  }
  ["b"]=>
  object(SensitiveParameterValue)#3 (0) {
  }
}
