--TEST--
Typed property on overloaded by-ref property
--FILE--
<?php

$a = new class {
	public int $foo = 1;

	function &__get($x) {
		return $this->foo;
	}

	function __set($x, $y) {
		print "ERROR: __set() called on by-ref __get()";
	}
};

$a->_ += 1;
var_dump($a->foo);

$a->_ .= "1";
var_dump($a->foo);

try {
	$a->_ .= "e50";
} catch (Error $e) { echo $e->getMessage(), "\n"; }
var_dump($a->foo);

$a->_--;
var_dump($a->foo);

--$a->_;
var_dump($a->foo);

$a->foo = PHP_INT_MAX;

try {
	$a->_++;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
echo gettype($a->foo),"\n";

try {
	++$a->_;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
echo gettype($a->foo),"\n";

?>
--EXPECT--
int(2)
int(21)
Cannot assign string to reference of type integer
int(21)
int(20)
int(19)
Cannot assign float to reference of type integer
integer
Cannot assign float to reference of type integer
integer
