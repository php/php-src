--TEST--
Bug #41693 (scandir() allows empty directory names)
--FILE--
<?php

try {
    var_dump(scandir(''));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: scandir(): Argument #1 ($directory) must not be empty
