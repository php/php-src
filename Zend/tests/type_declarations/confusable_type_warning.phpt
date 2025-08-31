--TEST--
Warnings for confusable types
--FILE--
<?php

namespace {
    function test1(integer $x) {}
    function test2(double $x) {}
    function test3(boolean $x) {}
    function test4(resource $x) {}
}

namespace Foo {
    use integer as foo;

    function test5(\integer $x) {}
    function test6(namespace\integer $x) {}
    function test7(foo $x) {}
    function test8(boolean $x) {}
}

namespace Foo {
    use integer;
    function test9(integer $x) {}
}

namespace {
    use integer as foo;

    function test10(\integer $x) {}
    function test11(namespace\integer $x) {}
    function test12(foo $x) {}
    function test13(boolean $x) {}
}

?>
--EXPECTF--
Warning: "integer" will be interpreted as a class name. Did you mean "int"? Write "\integer" to suppress this warning in %s on line %d

Warning: "double" will be interpreted as a class name. Did you mean "float"? Write "\double" to suppress this warning in %s on line %d

Warning: "boolean" will be interpreted as a class name. Did you mean "bool"? Write "\boolean" to suppress this warning in %s on line %d

Warning: "resource" is not a supported builtin type and will be interpreted as a class name. Write "\resource" to suppress this warning in %s on line %d

Warning: "boolean" will be interpreted as a class name. Did you mean "bool"? Write "\Foo\boolean" or import the class with "use" to suppress this warning in %s on line %d

Warning: "boolean" will be interpreted as a class name. Did you mean "bool"? Write "\boolean" to suppress this warning in %s on line %d
