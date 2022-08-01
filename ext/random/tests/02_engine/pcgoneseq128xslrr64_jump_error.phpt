--TEST--
Random: Engine: PcgOneseq128XslRr64: Jump with negative $advance
--FILE--
<?php

$engine = new \Random\Engine\PcgOneseq128XslRr64(1234);
$referenceEngine = new \Random\Engine\PcgOneseq128XslRr64(1234);

try {
    $engine->jump(-1);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}

if ($engine->generate() !== $referenceEngine->generate()) {
    die('failure: state changed');
}

die('success');
--EXPECT--
Random\Engine\PcgOneseq128XslRr64::jump(): Argument #1 ($advance) must be greater than or equal to 0
success
