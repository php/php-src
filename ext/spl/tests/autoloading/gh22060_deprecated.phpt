--TEST--
GH-22060 (Autoloader $this freed by the error handler of its own deprecation notice)
--FILE--
<?php

class Loader {
    public string $data = "loader-data";

    #[\Deprecated]
    public function load(string $class): void {
        echo $this->data, "\n";
    }
}

$obj = new Loader();
spl_autoload_register([$obj, 'load']);
unset($obj);

set_error_handler(function (int $no, string $str): bool {
    echo $str, "\n";
    foreach (spl_autoload_functions() as $loader) {
        spl_autoload_unregister($loader);
    }
    return true;
});

try {
    new NonExistentClass42();
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
Method Loader::load() is deprecated
loader-data
Error: Class "NonExistentClass42" not found
