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
} catch (EngineException $e) {
	echo "\nException: " . $e->getMessage() . " in " , $e->getFile() . " on line " . $e->getLine() . "\n";
}

var_dump($x->p);
?>
--EXPECTF--
Exception: Cannot access private property C::$p in /home/dmitry/php/php-master/Zend/tests/exception_014.php on line 8

Fatal error: Cannot access private property C::$p in /home/dmitry/php/php-master/Zend/tests/exception_014.php on line 13
