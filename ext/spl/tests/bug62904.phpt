--TEST--
Bug #62904 (Crash when cloning an object which inherits SplFixedArray)
--FILE--
<?php

class foo extends SplFixedArray {       
    public function __construct($size) {
    }
}

$x = new foo(2);

try {
    $z = clone $x;
} catch (Exception $e) {
    var_dump($e->getMessage());
}
--EXPECTF--
string(40) "The instance wasn't initialized properly"
