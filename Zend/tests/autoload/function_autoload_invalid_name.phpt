--TEST--
Function autoloaders are not invoked for invalid or empty function names
--FILE--
<?php
spl_autoload_register_function_loader(function (string $name) {
    echo "autoload($name)\n";
    if ($name === 'valid_fn') {
        function valid_fn() {
            echo "valid_fn called\n";
        }
    }
});

$f = 'foo bar';
try {
    $f();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump(function_exists('', true));
var_dump(function_exists('foo bar', true));

$g = 'valid_fn';
$g();
?>
--EXPECT--
Call to undefined function foo bar()
bool(false)
bool(false)
autoload(valid_fn)
valid_fn called
