--TEST--
Random: Randomizer: The engine property must be readonly
--FILE--
<?php

use Random\Engine\PcgOneseq128XslRr64;
use Random\Engine\Xoshiro256StarStar;
use Random\Randomizer;

$randomizer = new Randomizer(new PcgOneseq128XslRr64(1234));
$referenceRandomizer = new Randomizer(new PcgOneseq128XslRr64(1234));

try {
    $randomizer->engine = new Xoshiro256StarStar(1234);
} catch (Throwable $e) {
    echo $e->getMessage(), PHP_EOL;
}

for ($i = 0; $i < 10_000; $i++) {
    if ($randomizer->getInt(0, $i) !== $referenceRandomizer->getInt(0, $i)) {
        die("failure: state differs at {$i}");
    }
}

die('success');

?>
--EXPECT--
Cannot modify readonly property Random\Randomizer::$engine
success
