--TEST--
get_parent_class() tests
--FILE--
<?php

interface i {
    function test();
}

class foo implements i {
    function test() {
        var_dump(get_parent_class());
    }
}

class bar extends foo {
    function test_bar() {
        var_dump(get_parent_class($this));
    }
}

$bar = new bar;
$foo = new foo;

set_error_handler(function ($severity, $message, $file, $line) {
    throw new Exception($message);
});
try {
    $foo->test();
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
set_error_handler(null);

$foo->test();
$bar->test();
$bar->test_bar();

var_dump(get_parent_class($bar));
var_dump(get_parent_class($foo));
var_dump(get_parent_class("bar"));
var_dump(get_parent_class("foo"));
var_dump(get_parent_class("i"));

try {
    get_parent_class("");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    get_parent_class("[[[[");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    get_parent_class(" ");
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(get_parent_class(new stdclass));

try {
    get_parent_class(array());
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    get_parent_class(1);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECTF--
Calling get_parent_class() without arguments is deprecated

Deprecated: Calling get_parent_class() without arguments is deprecated in %s on line %d
bool(false)

Deprecated: Calling get_parent_class() without arguments is deprecated in %s on line %d
bool(false)
string(3) "foo"
string(3) "foo"
bool(false)
string(3) "foo"
bool(false)
bool(false)
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, string given
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, string given
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, string given
bool(false)
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, array given
get_parent_class(): Argument #1 ($object_or_class) must be an object or a valid class name, int given
Done
