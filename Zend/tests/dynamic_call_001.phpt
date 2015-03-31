--TEST--
Testing dynamic call to constructor (old-style)
--FILE--
<?php 

class foo { 
	public function foo() {
	}	
}

$a = 'foo';

$a::$a();

?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; foo has a deprecated constructor in %s on line %d

Fatal error: Non-static method foo::foo() cannot be called statically in %s on line %d
