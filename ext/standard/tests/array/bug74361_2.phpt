--TEST--
Bug #74361: Compaction in array_rand() violates COW (variation)
--FILE--
<?php

$array = range(0, 15);
for ($i = 0; $i <= 8; $i++) {
    unset($array[$i]);
}

foreach ($array as $x) {
    var_dump($x);
    array_rand($array, 1);
}

?>
--EXPECT--
int(9)
int(10)
int(11)
int(12)
int(13)
int(14)
int(15)
