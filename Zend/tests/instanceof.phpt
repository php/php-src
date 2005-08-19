--TEST--
instanceof shouldn't call __autoload
--FILE--
<?php
function __autoload($name) {
	echo("AUTOLOAD '$name'\n");
	eval("class $name {}");
}

class A {
}
$a = new A;
var_dump($a instanceof B);
var_dump($a instanceof A);
?>
--EXPECT--
bool(false)
bool(true)
