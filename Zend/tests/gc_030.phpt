--TEST--
GC 030: GC and exceptions in destructors
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
gc_collect_cycles();
?>
--EXPECTF--
Fatal error: Ignoring exception from foo::__destruct() while an exception is already active (Uncaught Exception in %sgc_030.php on line %d) in %sgc_030.php on line %d
