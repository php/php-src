--TEST--
Bug #66252 (Problems in AST evaluation invalidating valid parent:: reference)
--FILE--
<?php
class A {
	const HW = "this is A";
}
class B extends A {
	const BHW = parent::HW . " extended by B";
}
const C = B::BHW; 
echo C, "\n";
--EXPECT--
this is A extended by B
