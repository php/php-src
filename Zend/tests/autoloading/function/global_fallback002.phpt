--TEST--
Fallback to global function triggers autoloading once.
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
}

?>
--EXPECT--
function loader called with bar\foo
I am foo in global namespace.
