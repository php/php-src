--TEST--
GH-24989 (Use-after-free in GC when an exception is thrown, uncaught)
--CREDITS--
vi3tL0u1s
--FILE--
<?php
class D { function __destruct() { gc_collect_cycles(); } }
foreach (range(1, 2) as $v) {
    $x = [new D, throw new Exception];
}
?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
