--TEST--
Test to check static method calls syntax regression
--FILE--
<?php

class Foo {
	public static function function(){ echo __METHOD__, PHP_EOL; }
}

Foo::function();

Foo::
function();

Foo::
	 function();


Foo::
	 function(

);

echo "\nDone\n";
--EXPECTF--
Foo::function
Foo::function
Foo::function
Foo::function

Done
