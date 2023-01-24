--TEST--
GH-10292 (Made the default value of the first param of srand() and mt_srand() unknown
--FILE--
<?php

try {
    srand(mode: MT_RAND_MT19937);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    mt_srand(mode: MT_RAND_MT19937);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
srand(): Argument #1 ($seed) must be passed explicitly, because the default value is not known
mt_srand(): Argument #1 ($seed) must be passed explicitly, because the default value is not known
