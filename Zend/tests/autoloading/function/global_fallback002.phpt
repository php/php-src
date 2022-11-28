--TEST--
Fallback to global function should not trigger autoloading.
--FILE--
<?php
namespace {
    function loader($name) {
        echo $name, \PHP_EOL;
    }

    autoload_register_function('loader');
}

namespace bar {
	var_dump('Hello');
}
?>
--EXPECT--
string(5) "Hello"
