--TEST--
function_exist of local function returns false even with global one defined
--FILE--
<?php
namespace {
    function loader($name) {
        echo "function loader called with $name\n";
    }

    autoload_register_function('loader');

    function bar() {
        echo "I am bar in global namespace.\n";
    }
}

namespace foo {
    if (!function_exists(__NAMESPACE__.'\\bar')) {
        function bar() {
            echo "I'm bar in foo namespace";
        }
    }

    bar();
}

?>
--EXPECT--
function loader called with foo\function_exists
function loader called with foo\bar
I'm bar in foo namespace
