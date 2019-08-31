--TEST--
Typed property on overloaded by-ref property
--SKIPIF--
<?php if (PHP_INT_SIZE == 4) die("SKIP: 64 bit test"); ?>
--FILE--
<?php

$a = new class {
	public int $foo = 1;

	function &__get($x) {
		return $this->foo;
	}

	function __set($x, $y) {
		echo "set($y)\n";
	}
};

$a->_ += 1;
var_dump($a->foo);

$a->_ .= "1";
var_dump($a->foo);

$a->_ .= "e50";
var_dump($a->foo);

$a->_--;
var_dump($a->foo);

--$a->_;
var_dump($a->foo);

$a->foo = PHP_INT_MAX;

$a->_++;
var_dump($a->foo);

++$a->_;
var_dump($a->foo);

?>
--EXPECT--
set(2)
int(1)
set(11)
int(1)
set(1e50)
int(1)
set(0)
int(1)
set(0)
int(1)
set(9.2233720368548E+18)
int(9223372036854775807)
set(9.2233720368548E+18)
int(9223372036854775807)
