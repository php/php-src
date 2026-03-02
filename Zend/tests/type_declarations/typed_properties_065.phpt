--TEST--
Typed property on by-ref array dimension
--FILE--
<?php

$a = new class implements ArrayAccess {
    public int $foo = 1;

    function offsetExists($o): bool { return 1; }
    function &offsetGet($o): mixed { return $this->foo; }
    function offsetSet($o, $v): void { print "offsetSet($v)\n"; }
    function offsetUnset($o): void { print "offsetUnset() ?!?"; }
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

$a->foo = PHP_INT_MIN;

try {
        $a[0]--;
} catch (Error $e) { echo $e->getMessage(), "\n"; }
echo gettype($a->foo),"\n";

try {
    --$a[0];
} catch (Error $e) { echo $e->getMessage(), "\n"; }
echo gettype($a->foo),"\n";

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
Cannot decrement a reference held by property ArrayAccess@anonymous::$foo of type int past its minimal value
integer
Cannot decrement a reference held by property ArrayAccess@anonymous::$foo of type int past its minimal value
integer
Cannot increment a reference held by property ArrayAccess@anonymous::$foo of type int past its maximal value
integer
Cannot increment a reference held by property ArrayAccess@anonymous::$foo of type int past its maximal value
integer
