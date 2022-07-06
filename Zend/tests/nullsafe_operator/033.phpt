--TEST--
Test nullsafe operator in encaps vars
--FILE--
<?php

class Foo {
    public $bar = 'bar';

    function qux() {
        return 'qux';
    }
}

$null = null;
$foo = new Foo();

var_dump("{$null?->foo}");
var_dump("{$null?->bar()}");
var_dump("$null?->foo");
var_dump("$null?->bar()");

var_dump("{$foo?->bar}");
var_dump("{$foo?->baz}");
var_dump("{$foo?->qux()}");
try {
    var_dump("{$foo?->quux()}");
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

var_dump("$foo?->bar");
var_dump("$foo?->baz");
var_dump("$foo?->qux()");
try {
    var_dump("$foo?->quux()");
} catch (Throwable $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
string(0) ""
string(0) ""
string(0) ""
string(2) "()"
string(3) "bar"

Warning: Undefined property: Foo::$baz in %s.php on line 20
string(0) ""
string(3) "qux"
string(36) "Call to undefined method Foo::quux()"
string(3) "bar"

Warning: Undefined property: Foo::$baz in %s.php on line 29
string(0) ""

Warning: Undefined property: Foo::$qux in %s.php on line 30
string(2) "()"

Warning: Undefined property: Foo::$quux in %s.php on line 32
string(2) "()"
