--TEST--
Using unset(), isset(), empty() with an illegal array offset throws
--FILE--
<?php

$ary = [];
try {
    unset($ary[[]]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    isset($ary[[]]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    empty($ary[[]]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Illegal offset type in unset
Illegal offset type in isset or empty
Illegal offset type in isset or empty
