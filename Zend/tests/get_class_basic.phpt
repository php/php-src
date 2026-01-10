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

set_error_handler(function ($severity, $message, $file, $line) {
    throw new Exception($message);
});
try {
    $f1->bar();
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
set_error_handler(null);

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
--EXPECTF--
Calling get_class() without arguments is deprecated

Deprecated: Calling get_class() without arguments is deprecated in %s on line %d
string(3) "foo"
get_class() without arguments must be called from within a class
get_class(): Argument #1 ($object) must be of type object, string given
string(3) "foo"
string(4) "foo2"
get_class(): Argument #1 ($object) must be of type object, null given
Done
