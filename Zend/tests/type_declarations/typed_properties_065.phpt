--TEST--
Typed property on by-ref array dimension
--FILE--
<?php

$a = new class implements ArrayAccess {
	public int $foo = 1;

	function offsetExists($o) { return 1; }
	function &offsetGet($o) { return $this->foo; }
	function offsetSet($o, $v) { print "offsetSet() must not be called"; }
	function offsetUnset($o) { print "offsetUnset() ?!?"; }
};

$a[0] += 1;
var_dump($a->foo);

$a[0] .= "1";
var_dump($a->foo);

try {
	$a[0] .= "e50";
} catch (Error $e) { echo $e->getMessage(), "\n"; }
var_dump($a->foo);

$a[0]--;
var_dump($a->foo);

--$a[0];
var_dump($a->foo);

$a->foo = PHP_INT_MAX;

try {
	$a[0]++;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
echo gettype($a->foo),"\n";

try {
	++$a[0];
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
