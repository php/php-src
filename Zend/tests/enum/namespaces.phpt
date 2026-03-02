--TEST--
Enum namespace
--FILE--
<?php

namespace Foo {
    enum Bar {
        case Baz;

        public function dump() {
            var_dump(Bar::Baz);
        }
    }

    function dumpBar() {
        Bar::Baz->dump();
    }
}

namespace {
    use Foo\Bar;

    \Foo\dumpBar();
    \Foo\Bar::Baz->dump();
    Bar::Baz->dump();
}

?>
--EXPECT--
enum(Foo\Bar::Baz)
enum(Foo\Bar::Baz)
enum(Foo\Bar::Baz)
