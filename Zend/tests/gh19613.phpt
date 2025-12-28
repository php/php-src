--TEST--
GH-19613: Invalidated array iterator pointer after array separation
--FILE--
<?php

$a = [1];
$i = 0;

foreach ($a as &$v) {
    $a[0] = $a;
    foreach ($v as &$w) {
        $w = $a;

        if ($i++ == 64) {
            die("===DONE===\n");
        }
    }
}

?>
--EXPECT--
===DONE===
