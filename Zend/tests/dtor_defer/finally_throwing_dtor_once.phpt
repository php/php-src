--TEST--
Deferred destructor: a throwing destructor deferred in a finally runs exactly once
--FILE--
<?php
class D {
    function __destruct() {
        echo "dtor\n";
        throw new Exception("boom");
    }
}
function g() {
    try {
        return 1;
    } finally {
        $x = new D();
        unset($x);
    }
}
g();
echo "after g\n";
?>
--EXPECTF--
dtor

Fatal error: Uncaught Exception: boom in %s:%d
Stack trace:
#0 %s(%d): D->__destruct()
#1 %s(%d): g()
#2 {main}
  thrown in %s on line %d
