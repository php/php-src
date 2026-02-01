--TEST--
Failed assertion for assignment in expression context
--CREDITS--
Matteo Beccati (mbeccati)
--FILE--
<?php

function foo() {
    return ['bar' => 'baz'];
}

var_dump(($v = foo())['bar'], $v);

?>
--EXPECT--
string(3) "baz"
array(1) {
  ["bar"]=>
  string(3) "baz"
}
