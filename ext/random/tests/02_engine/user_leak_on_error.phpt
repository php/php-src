--TEST--
Random: Engine: User: Empty strings do not leak
--FILE--
<?php

use Random\Engine;
use Random\Randomizer;
use Random\BrokenRandomEngineError;

final class MyEngine implements Engine
{
    public function generate(): string
    {
        // Create a non-interned empty string.
        return preg_replace('/./', '', random_bytes(4));
    }
}

$randomizer = new Randomizer(new MyEngine());
try {
    $randomizer->getBytes(64);
} catch (BrokenRandomEngineError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
A random engine must return a non-empty string
