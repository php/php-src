--TEST--
063: Old-style constructors in namesapces (not supported!)
--FILE--
<?php
namespace Foo;
class Bar {
    function Bar() {
        echo "ok\n";
    }
}
new Bar();
echo "ok\n";
--EXPECT--
ok
