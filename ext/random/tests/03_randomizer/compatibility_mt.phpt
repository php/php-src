--TEST--
Random: Randomizer: Compatibility: Mt19937
--FILE--
<?php

$randomizer = new \Random\Randomizer(new \Random\Engine\Mt19937(1234, \MT_RAND_PHP));
\mt_srand(1234, \MT_RAND_PHP);
for ($i = 0; $i < 1000; $i++) {
    if ($randomizer->nextInt() !== \mt_rand()) {
        die('failure');
    }
}

$randomizer = new \Random\Randomizer(new \Random\Engine\Mt19937(1234, \MT_RAND_MT19937));
\mt_srand(1234, \MT_RAND_MT19937);
for ($i = 0; $i < 1000; $i++) {
    if ($randomizer->nextInt() !== \mt_rand()) {
        die('failure');
    }
}

die('success');
--EXPECT--
success
