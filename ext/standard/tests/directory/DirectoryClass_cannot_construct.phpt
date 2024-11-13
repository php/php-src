--TEST--
Cannot directly instantiate Directory class.
--FILE--
<?php

try {
    $d = new Directory();
    var_dump($d);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Cannot directly construct Directory, use dir() instead
