--TEST--
ReflectionConstant with namespace
--FILE--
<?php

namespace Foo {
    const C = 42;
}

namespace {
    const C = 43;

    var_dump(new \ReflectionConstant('C'));
    var_dump(new \ReflectionConstant('\\C'));
    var_dump(new \ReflectionConstant('Foo\\C'));
    var_dump(new \ReflectionConstant('\\Foo\\C'));
    var_dump((new \ReflectionConstant('C'))->getNamespaceName());
    var_dump((new \ReflectionConstant('\\C'))->getNamespaceName());
    var_dump((new \ReflectionConstant('Foo\\C'))->getNamespaceName());
    var_dump((new \ReflectionConstant('\\Foo\\C'))->getNamespaceName());
    var_dump((new \ReflectionConstant('C'))->getShortName());
    var_dump((new \ReflectionConstant('\\C'))->getShortName());
    var_dump((new \ReflectionConstant('Foo\\C'))->getShortName());
    var_dump((new \ReflectionConstant('\\Foo\\C'))->getShortName());
}

?>
--EXPECT--
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(1) "C"
}
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(2) "\C"
}
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(5) "Foo\C"
}
object(ReflectionConstant)#1 (1) {
  ["name"]=>
  string(6) "\Foo\C"
}
string(0) ""
string(0) ""
string(3) "Foo"
string(3) "Foo"
string(1) "C"
string(1) "C"
string(1) "C"
string(1) "C"
