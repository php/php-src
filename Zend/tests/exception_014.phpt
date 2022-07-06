--TEST--
Exceptions on improper access to static class properties
--FILE--
<?php
class C {
    private $p = 0;
}

$x = new C;
try {
    var_dump($x->p);
} catch (Error $e) {
    echo "\nException: " . $e->getMessage() . " in " , $e->getFile() . " on line " . $e->getLine() . "\n";
}

var_dump($x->p);
?>
--EXPECTF--
Exception: Cannot access private property C::$p in %sexception_014.php on line %d

Fatal error: Uncaught Error: Cannot access private property C::$p in %sexception_014.php:%d
Stack trace:
#0 {main}
  thrown in %sexception_014.php on line %d
