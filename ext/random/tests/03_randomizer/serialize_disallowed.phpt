--TEST--
Random: Randomizer: Serialization of the Randomizer fails if the engine is not serializable
--FILE--
<?php

use Random\Engine\Secure;
use Random\Randomizer;

try {
    serialize(new Randomizer(new Secure()));
} catch (Exception $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Serialization of 'Random\Engine\Secure' is not allowed
