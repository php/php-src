--TEST--
Exceptions on improper access to static class properties
--FILE--
<?php
class C {
    static private $p = 0;
}

try {
    var_dump(C::$a);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), ' in ', $e->getFile(), ' on line ', $e->getLine(), "\n";
}

try {
    var_dump(C::$p);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), ' in ', $e->getFile(), ' on line ', $e->getLine(), "\n";
}

try {
    unset(C::$a);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), ' in ', $e->getFile(), ' on line ', $e->getLine(), "\n";
}

var_dump(C::$a);
?>
--EXPECTF--
Error: Access to undeclared static property C::$a in %s on line %d
Error: Cannot access private property C::$p in %sexception_013.php on line 13
Error: Attempt to unset static property C::$a in %sexception_013.php on line 19

Fatal error: Uncaught Error: Access to undeclared static property C::$a in %s:%d
Stack trace:
#0 {main}
  thrown in %sexception_013.php on line 24
