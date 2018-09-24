--TEST--
Typed property on by-ref array dimension
--FILE--
<?php

$a = new class implements ArrayAccess {
	public int $foo = 1;

	function offsetExists($o) { return 1; }
	function &offsetGet($o) { return $this->foo; }
	function offsetSet($o, $v) { print "offsetSet($v)\n"; }
	function offsetUnset($o) { print "offsetUnset() ?!?"; }
};

$a[0] += 1;
var_dump($a->foo);

$a[0] .= "1";
var_dump($a->foo);

$a[0] .= "e50";
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
offsetSet(2)
int(1)
offsetSet(11)
int(1)
offsetSet(1e50)
int(1)
int(0)
int(-1)
Cannot assign float to reference held by property class@anonymous::$foo of type int
integer
Cannot assign float to reference held by property class@anonymous::$foo of type int
integer
