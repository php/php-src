--TEST--
Test escapeshellarg() allowed argument length
--FILE--
<?php
ini_set('memory_limit', -1);
$var_2  = str_repeat('A', 1024*1024*64);

try {
    escapeshellarg($var_2);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
===DONE===
--EXPECTF--
ValueError: Argument exceeds the allowed length of %d bytes
===DONE===
