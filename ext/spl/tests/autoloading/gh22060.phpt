--TEST--
GH-22060 (Class autoloader $this freed via spl_autoload_unregister during dispatch)
--FILE--
<?php

class Loader {
    public string $data = "loader-data";

    public function load(string $class): void {
        spl_autoload_unregister([$this, 'load']);
        echo $this->data, "\n";
    }
}

$obj = new Loader();
spl_autoload_register([$obj, 'load']);
unset($obj);

try {
    new NonExistentClass42();
} catch (\Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
?>
--EXPECT--
loader-data
Error: Class "NonExistentClass42" not found
