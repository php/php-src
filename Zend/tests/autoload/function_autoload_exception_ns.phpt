--TEST--
Exception during function autoloading via namespace fallback
--FILE--
<?php
namespace App;

\spl_autoload_register_function_loader(function (string $name) {
    throw new \RuntimeException("Autoload failed for: $name");
});

try {
    some_func();
} catch (\RuntimeException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Autoload failed for: App\some_func
