--TEST--
Function autoloading recursion prevention
--FILE--
<?php
spl_autoload_register_function_loader(function (string $name) {
    echo "Autoloading: $name\n";
    if ($name === 'recursive_func') {
        try {
            recursive_func();
        } catch (Error $e) {
            echo "Caught: " . $e->getMessage() . "\n";
        }
    }
});

try {
    recursive_func();
} catch (Error $e) {
    echo "Final: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Autoloading: recursive_func
Caught: Call to undefined function recursive_func()
Final: Call to undefined function recursive_func()
