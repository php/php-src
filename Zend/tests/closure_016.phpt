--TEST--
Closure 016: closures and is_callable()
--FILE--
<?php
class Foo {
    function __invoke() {
        echo "Hello World!\n";
    }
}

function foo() {
    return function() {
        echo "Hello World!\n";
    };
}
$test = new Foo;
var_dump(is_callable($test, true, $name));
echo $name."\n";
var_dump(is_callable($test, false, $name));
echo $name."\n";
var_dump(is_callable(array($test,"__invoke"), true, $name));
echo $name."\n";
var_dump(is_callable(array($test,"__invoke"), false, $name));
echo $name."\n";
$test = foo();
var_dump(is_callable($test, true, $name));
echo $name."\n";
var_dump(is_callable($test, false, $name));
echo $name."\n";
var_dump(is_callable(array($test,"__invoke"), true, $name));
echo $name."\n";
var_dump(is_callable(array($test,"__invoke"), false, $name));
echo $name."\n";
?>
--EXPECT--
bool(true)
Foo::__invoke
bool(true)
Foo::__invoke
bool(true)
Foo::__invoke
bool(true)
Foo::__invoke
bool(true)
Closure::__invoke
bool(true)
Closure::__invoke
bool(true)
Closure::__invoke
bool(true)
Closure::__invoke
