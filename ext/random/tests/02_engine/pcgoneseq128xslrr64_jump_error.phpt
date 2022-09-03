--TEST--
Random: Engine: PcgOneseq128XslRr64: Negative jumps must be rejected
--FILE--
<?php

use Random\Engine\PcgOneseq128XslRr64;

$engine = new PcgOneseq128XslRr64(1234);
$referenceEngine = new PcgOneseq128XslRr64(1234);

try {
    $engine->jump(-1);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

for ($i = 0; $i < 10_000; $i++) {
    if ($engine->generate() !== $referenceEngine->generate()) {
        die("failure: state differs at {$i}");
    }
}

die('success');

?>
--EXPECT--
Random\Engine\PcgOneseq128XslRr64::jump(): Argument #1 ($advance) must be greater than or equal to 0
success
