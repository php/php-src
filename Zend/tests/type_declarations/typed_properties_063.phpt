--TEST--
Typed property on by-ref variable
--FILE--
<?php

$a = new class {
	public int $foo = 1;
};

$_ = &$a->foo;

$_ += 1;
var_dump($a->foo);

$_ .= "1";
var_dump($a->foo);

try {
	$_ .= "e50";
} catch (Error $e) { echo $e->getMessage(), "\n"; }
var_dump($a->foo);

$_--;
var_dump($a->foo);

--$_;
var_dump($a->foo);

$a->foo = PHP_INT_MAX;

try {
	$_++;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
var_dump($a->foo);

try {
	++$_;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
var_dump($a->foo);

try {
	$_ = [];
} catch (Error $e) { echo $e->getMessage(), "\n"; }
var_dump($a->foo);

$_ = 1;
var_dump($a->foo);

?>
--EXPECT--
int(2)
int(21)
Cannot assign string to reference of type integer
int(21)
int(20)
int(19)
Cannot assign float to reference of type integer
int(9223372036854775807)
Cannot assign float to reference of type integer
int(9223372036854775807)
Cannot assign array to reference of type integer
int(9223372036854775807)
int(1)
