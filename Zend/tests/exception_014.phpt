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
Exception: Private property C::$p cannot be accessed from the global scope in %s on line %d

Fatal error: Uncaught Error: Private property C::$p cannot be accessed from the global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %sexception_014.php on line %d
