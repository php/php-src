--TEST--
Bug #71202 (Autoload function registered by another not activated immediately)
--FILE--
<?php

function inner_autoload ($name){
	if ($name == 'A') {
		class A {
			function __construct(){
				echo "okey, ";
			}
		}
	} else {
		class B {
			function __construct() {
				die("error");
			}
		}
	}
}

spl_autoload_register(function ($name) {
	if ($name == 'A') {
		spl_autoload_register("inner_autoload");
	} else {
		spl_autoload_unregister("inner_autoload");
	}
});

$c = new A();
try {
	$c = new B();
} catch (Error $e) {
	echo "done";
}
?>
--EXPECT--
okey, done
