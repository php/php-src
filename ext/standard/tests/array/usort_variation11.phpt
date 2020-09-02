--TEST--
Test usort() function : usage variations - malformed comparison function returning boolean
--FILE--
<?php

function ucmp($a, $b) {
    return $a > $b;
}

$range = array(2, 4, 8, 16, 32, 64, 128);

foreach ($range as $r) {
    $backup = $array = range(0, $r);
    shuffle($array);
    usort($array, "ucmp");
    if ($array != $backup) {
        var_dump($array);
        var_dump($backup);
        die("Array not sorted (usort)");
    }

    shuffle($array);
    $array = array_flip($array);
    uksort($array, "ucmp");
    $array = array_keys($array);
    if ($array != $backup) {
        var_dump($array);
        var_dump($backup);
        die("Array not sorted (uksort)");
    }

    shuffle($array);
    $array = array_combine($array, $array);
    uasort($array, "ucmp");
    $array = array_keys($array);
    if ($array != $backup) {
        var_dump($array);
        var_dump($backup);
        die("Array not sorted (uasort)");
    }
}
echo "okey";

?>
--EXPECTF--
Deprecated: usort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uksort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uasort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: usort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uksort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uasort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: usort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uksort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uasort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: usort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uksort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uasort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: usort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uksort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uasort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: usort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uksort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uasort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: usort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uksort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d

Deprecated: uasort(): Returning bool from comparison function is deprecated, return an integer less than, equal to, or greater than zero in %s on line %d
okey
