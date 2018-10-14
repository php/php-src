--TEST--
Bug #62904 (Crash when cloning an object which inherits SplFixedArray)
--FILE--
<?php

class foo extends SplFixedArray {
    public function __construct($size) {
    }
}

$x = new foo(2);

$z = clone $x;
echo "No crash.";
--EXPECTF--
No crash.
