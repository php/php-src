--TEST--
Bug #80072: Cyclic unserialize in TMPVAR operand may leak
--FILE--
<?php

try {
    $s = 'O:8:"stdClass":1:{s:1:"x";r:1;}';
    unserialize($s) % gc_collect_cycles();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$a[]=&$a == $a=&$b > gc_collect_cycles();

?>
--EXPECT--
TypeError: Unsupported operand types: stdClass % int
