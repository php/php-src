--TEST--
Exception during function autoloading
--FILE--
<?php
spl_autoload_register_function_loader(function (string $name) {
    throw new RuntimeException("Autoload failed for: $name");
});

try {
    missing_func();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

// failed autoloads are not cached, so the loader runs again
try {
    missing_func();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Autoload failed for: missing_func
Autoload failed for: missing_func
