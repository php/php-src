--TEST--
GH-13279: Instable array during in-place modification in uksort
--FILE--
<?php

// Make sure the array is not const
$array = [];
$array['a'] = 1;
$array['b'] = 2;

uksort($array, function ($a, $b) use (&$array) {
    return $array[$a] - $array[$b];
});

?>
===DONE===
--EXPECT--
===DONE===
