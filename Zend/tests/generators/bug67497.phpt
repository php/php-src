--TEST--
Bug #67467: eval with parse error causes segmentation fault in generator
--FILE--
<?php

function gen() {
    $a = 1;
    yield $a;
}

try {
    eval('abc');
} catch (ParseError $ex) {
}

$values = gen();
$values->next();

?>
===DONE===
--EXPECT--
===DONE===
