--TEST--
GH-25299 (Exception string code is lost after unserialize())
--CREDITS--
tacman
--FILE--
<?php

class MyException extends Exception {
    protected $code = 'HY000';
}

$e = unserialize(serialize(new MyException()));
var_dump($e->getCode());

$e = unserialize('O:9:"Exception":1:{s:7:"' . "\0*\0" . 'code";a:0:{}}');
var_dump($e->getCode());

?>
--EXPECTF--
string(5) "HY000"

Warning: Undefined property: Exception::$code in %s on line %d
NULL
