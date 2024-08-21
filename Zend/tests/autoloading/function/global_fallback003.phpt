--TEST--
Fallback to non-existent function triggers autoloading once in namespace, once in global.
--FILE--
<?php

namespace {
    function loader($name) {
        echo "function loader called with $name\n";
    }

    autoload_register_function('loader');
}

namespace bar {
    try {
        non_existent_function();
    }
    catch (\Error $e) {
        echo "Error correctly caught: " . $e->getMessage() . "\n";
    }
}

?>
--EXPECT--
function loader called with bar\non_existent_function
function loader called with non_existent_function
Error correctly caught: Call to undefined function bar\non_existent_function()
