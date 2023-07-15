--TEST--
Fallback to global function should trigger autoloading only once per namespace.
--FILE--
<?php

namespace {
    function loader($name) {
        echo "function loader called with $name\n";
    }

    autoload_register_function('loader');

    function foo() {
        echo "I am foo in global namespace.\n";
    }
}

namespace bar {
    foo();

    for ($i = 0; $i < 3; $i += 1) {
        foo();
    }
}
namespace bar {
    foo();
}

namespace Quux {
    foo();
}

?>
--EXPECT--
function loader called with bar\foo
I am foo in global namespace.
I am foo in global namespace.
I am foo in global namespace.
I am foo in global namespace.
I am foo in global namespace.
function loader called with Quux\foo
I am foo in global namespace.
