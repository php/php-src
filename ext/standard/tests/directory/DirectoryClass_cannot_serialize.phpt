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
--EXPECTF--
string(%d) "O:9:"Directory":2:{s:4:"path";s:%d:"%s";s:6:"handle";i:%d;}"
