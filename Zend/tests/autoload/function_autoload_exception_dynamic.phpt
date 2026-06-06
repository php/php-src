--TEST--
Exception during function autoloading via dynamic call
--FILE--
<?php
spl_autoload_register_function_loader(function (string $name) {
    throw new RuntimeException("Autoload failed for: $name");
});

$f = 'missing_func';
try {
    $f();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Autoload failed for: missing_func
