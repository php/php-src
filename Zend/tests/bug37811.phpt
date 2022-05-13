--TEST--
Bug #37811 (define not using toString on objects)
--FILE--
<?php

class TestClass
{
    function __toString()
    {
        return "Foo";
    }
}

define("Bar", new TestClass);
var_dump(Bar);
var_dump((string) Bar);

define("Baz", new stdClass);
var_dump(Baz);
try {
    var_dump((string) Baz);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(TestClass)#1 (0) {
}
string(3) "Foo"
object(stdClass)#2 (0) {
}
Object of class stdClass could not be converted to string
