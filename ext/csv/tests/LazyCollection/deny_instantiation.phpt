--TEST--
Csv\LazyLaxCollection must not be instantiable
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

try {
    new Csv\LazyLaxCollection();
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
Error: Cannot instantiate Csv\LazyLaxCollection class
