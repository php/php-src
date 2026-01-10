--TEST--
GH-19765: object_properties_load() bypasses readonly property checks
--FILE--
<?php

use Random\Engine\Mt19937;
use Random\Engine\PcgOneseq128XslRr64;
use Random\Randomizer;

try {
    $r = new Randomizer(new Mt19937());
    $r->__unserialize([['engine' => new PcgOneseq128XslRr64()]]);
} catch (Exception $error) {
    echo $error->getMessage() . "\n";
}
var_dump($r->engine::class);

?>
--EXPECT--
Invalid serialization data for Random\Randomizer object
string(21) "Random\Engine\Mt19937"
