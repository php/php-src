--TEST--
Random: NumberGenerator: MersenneTwister: compatibility
--FILE--
<?php

$mt = new Random\NumberGenerator\MersenneTwister(1234, \MT_RAND_MT19937);
\mt_srand(1234, \MT_RAND_MT19937);
for ($i = 0; $i < 1000; $i++) {
    if (($mt->generate() >> 1) !== \mt_rand()) {
        die('failure');
    }
}

$mt = new Random\NumberGenerator\MersenneTwister(1234, \MT_RAND_PHP);
\mt_srand(1234, \MT_RAND_PHP);
for ($i = 0; $i < 1000; $i++) {
    if (($mt->generate() >> 1) !== \mt_rand()) {
        die('failure');
    }
}

die('success');
?>
--EXPECT--
success
