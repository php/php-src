--TEST--
Using unset(), isset(), empty() with an illegal array offset throws
--FILE--
<?php

$ary = [];
try {
    unset($ary[[]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    isset($ary[[]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    empty($ary[[]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Cannot unset offset of type array on array
TypeError: Cannot access offset of type array in isset or empty
TypeError: Cannot access offset of type array in isset or empty
