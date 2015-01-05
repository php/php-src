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
Fatal error: Non-static method foo::foo() cannot be called statically in %s on line %d
