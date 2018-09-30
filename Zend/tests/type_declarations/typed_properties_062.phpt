--TEST--
Typed property on by-ref property
--FILE--
<?php

$a = new class {
	public int $foo = 1;
	public $_;
};

$a->_ = &$a->foo;

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

$a->foo = PHP_INT_MIN;

try {
	$a->_--;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
echo gettype($a->foo),"\n";

try {
	--$a->_;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
echo gettype($a->foo),"\n";

$a->foo = PHP_INT_MAX;

try {
	$a->_++;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
echo gettype($a->foo),"\n";

try {
	++$a->_;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
echo gettype($a->foo),"\n";

$a->_ = 0;
try {
	$a->_ = [];
} catch (Error $e) { echo $e->getMessage(), "\n"; }
var_dump($a->foo);

$a->_ = 1;
var_dump($a->foo);

?>
--EXPECT--
int(2)
int(21)
Cannot assign string to reference held by property class@anonymous::$foo of type int
int(21)
int(20)
int(19)
Cannot decrement a reference held by property class@anonymous::$foo of type int past its minimal value
integer
Cannot decrement a reference held by property class@anonymous::$foo of type int past its minimal value
integer
Cannot increment a reference held by property class@anonymous::$foo of type int past its maximal value
integer
Cannot increment a reference held by property class@anonymous::$foo of type int past its maximal value
integer
Cannot assign array to reference held by property class@anonymous::$foo of type int
int(0)
int(1)
