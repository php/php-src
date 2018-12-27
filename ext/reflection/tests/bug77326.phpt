--TEST--
ReflectionClass::getReflectionConstant()
--CREDITS--
Yanlong He <yanlong.hee@gmail.com>
--FILE--
<?php
class C {
}

$rc = new ReflectionClass('C');
try {
	var_dump($rc->getReflectionConstant("XXX"));
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Constant C::XXX does not exist
NULL
