--TEST--
Bug #42976 (Crash when constructor for newInstance() or newInstanceArgs() fails)
--FILE--
<?php

Class C {
    function __construct(&$x) {
        $x = "x.changed";
    }
}

$x = "x.original";
new C($x); // OK
var_dump($x);

$rc = new ReflectionClass('C');
$x = "x.original";
$rc->newInstance($x); // causes crash
var_dump($x);
$x = "x.original";
$rc->newInstanceArgs(array($x)); // causes crash
var_dump($x);

echo "Done\n";
?>
--EXPECTF--
string(9) "x.changed"

Warning: C::__construct(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
string(10) "x.original"

Warning: C::__construct(): Argument #1 ($x) must be passed by reference, value given in %s on line %d
string(10) "x.original"
Done
