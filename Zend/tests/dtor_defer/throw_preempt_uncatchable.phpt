--TEST--
Deferred destructor: an uncatchable throwing destructor pending at an explicit throw or internal call preempts it positionally instead of being held to shutdown
--FILE--
<?php
class D {
    public function __destruct() { throw new Exception("dtor"); }
}
$d = new D();
$e = new Exception("main");
unset($d);
throw $e;
?>
--EXPECTF--
Fatal error: Uncaught Exception: dtor in %s:%d
Stack trace:
#0 %s(%d): D->__destruct()
#1 {main}
  thrown in %s on line %d
