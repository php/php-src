--TEST--
GC 030: GC and exceptions in destructors — under test_scheduler
--INI--
test_scheduler.enable=1
zend.enable_gc=1
--EXTENSIONS--
test_scheduler
--FILE--
<?php
class foo {
    public $foo;

    public function __destruct() {
        throw new Exception("foobar");
    }
}

$f1 = new foo;
$f2 = new foo;
$f1->foo = $f2;
$f2->foo = $f1;
unset($f1, $f2);
/* The destructors run in the GC coroutine, so their traces no longer pass
 * through the gc_collect_cycles() frame of the caller. */
gc_collect_cycles();
?>
--EXPECTF--
Fatal error: Uncaught Exception: foobar in %s:%d
Stack trace:
#0 [internal function]: foo->__destruct()
#1 {main}

Next Exception: foobar in %s:%d
Stack trace:
#0 [internal function]: foo->__destruct()
#1 {main}
  thrown in %s on line %d
