--TEST--
Random: Randomizer: The Mt19937 engine is a drop-in replacement for mt_rand()
--FILE--
<?php

use Random\Randomizer;
use Random\Engine\Mt19937;

echo "MT_RAND_PHP", PHP_EOL;

$randomizer = new Randomizer(new Mt19937(1234, MT_RAND_PHP));
mt_srand(1234, MT_RAND_PHP);

for ($i = 0; $i < 10_000; $i++) {
    if ($randomizer->nextInt() !== mt_rand()) {
        die("failure: state differs at {$i} for nextInt()");
    }
}

for ($i = 0; $i < 10_000; $i++) {
    if ($randomizer->getInt(0, $i) !== mt_rand(0, $i)) {
        die("failure: state differs at {$i} for getInt()");
    }
}

echo "MT_RAND_MT19937", PHP_EOL;

$randomizer = new Randomizer(new Mt19937(1234, MT_RAND_MT19937));
mt_srand(1234, MT_RAND_MT19937);

for ($i = 0; $i < 10_000; $i++) {
    if ($randomizer->nextInt() !== mt_rand()) {
        die("failure: state differs at {$i} for nextInt()");
    }
}

for ($i = 0; $i < 10_000; $i++) {
    if ($randomizer->getInt(0, $i) !== mt_rand(0, $i)) {
        die("failure: state differs at {$i} for getInt()");
    }
}

die('success');

?>
--EXPECTF--
MT_RAND_PHP

Deprecated: Constant MT_RAND_PHP is deprecated in %s on line %d

Deprecated: The MT_RAND_PHP variant of Mt19937 is deprecated in %s on line %d

Deprecated: Constant MT_RAND_PHP is deprecated in %s on line %d

Deprecated: The MT_RAND_PHP variant of Mt19937 is deprecated in %s on line %d
MT_RAND_MT19937
success
