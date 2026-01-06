--TEST--
use SomeClass from Namespace syntax
--FILE--
<?php

namespace Test {
    class MyClass {
        public static function greet() {
            return "Hello from Test\\MyClass";
        }
    }
}

namespace Foo\Bar {
    class Baz {
        public static function info() {
            return "Foo\\Bar\\Baz";
        }
    }
}

namespace {
    use MyClass from Test;
    use Baz from Foo\Bar;

    echo MyClass::greet() . "\n";
    echo Baz::info() . "\n";
}

?>
--EXPECT--
Hello from Test\MyClass
Foo\Bar\Baz
