--TEST--
Adding a return type during inheritance is allowed
--FILE--
<?php

interface One {
    public function a();
    public function b();
    public function c();
    public function d();
}

interface Two extends One {
    public function a() : stdClass;
    public function c() : callable;
    public function b() : array;
    public function d() : int;
}

?>
Done
--EXPECT--
Done
