--TEST--
Bug #66252 (Problems in AST evaluation invalidating valid parent:: reference)
--INI--
opcache.enable=0
opcache.enable_cli=0
opcache.optimization_level=0
--FILE--
<?php
class A {
	const CONST_A = "this is A";
}

class B extends A {
	const CONST_B = parent::CONST_A . " extended by B";
}
const C = B::CONST_B; 
echo C, "\n";
?>
--EXPECT--
this is A extended by B