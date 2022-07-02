--TEST--
get_class() tests
--FILE--
<?php

class foo {
    function bar () {
        var_dump(get_class());
    }
    function testNull ()
    {
        try {
            var_dump(get_class(null));
        } catch (TypeError $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

class foo2 extends foo {
}

$f1 = new foo;
$f2 = new foo2;

$f1->bar();
$f2->bar();

try {
    var_dump(get_class());
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(get_class("qwerty"));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

var_dump(get_class($f1));
var_dump(get_class($f2));

$f1->testNull();

echo "Done\n";
?>
--EXPECT--
string(3) "foo"
string(3) "foo"
get_class() without arguments must be called from within a class
get_class(): Argument #1 ($object) must be of type object, string given
string(3) "foo"
string(4) "foo2"
get_class(): Argument #1 ($object) must be of type object, null given
Done
