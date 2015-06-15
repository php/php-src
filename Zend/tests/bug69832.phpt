--TEST--
Bug #69832 (Assertion failed in zend_compile_const_expr_magic_const)
--FILE--
<?php

class Test {
	public $foo = [Bar::A, __CLASS__];
}

eval(<<<'PHP'
class Bar {
	const A = 1;
}
PHP
);

var_dump((new Test)->foo);

?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(4) "Test"
}
