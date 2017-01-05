--TEST--
Bug #72846 (getConstant for a array constant with constant values returns NULL/NFC/UKNOWN)
--FILE--
<?php

namespace Some {

	abstract class A
	{
		const ONE = '1';
		const TWO = '2';

		const CONST_NUMBERS = [
			self::ONE,
			self::TWO,
		];

		const NUMBERS = [
			'1',
			'2',
		];
	}

	class B extends A
	{
	}

	$ref = new \ReflectionClass('Some\B');

	var_dump($ref->getConstant('ONE'));
	var_dump($ref->getConstant('CONST_NUMBERS'));
	var_dump($ref->getConstant('NUMBERS'));
}
?>
--EXPECT--
string(1) "1"
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "2"
}
