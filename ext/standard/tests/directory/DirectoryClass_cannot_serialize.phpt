--TEST--
Cannot serialize instance of Directory class constructed via Reflection.
--FILE--
<?php

$d = dir(__DIR__);
try {
    $s = serialize($d);
    var_dump($s);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Exception: Serialization of 'Directory' is not allowed
