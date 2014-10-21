--TEST--
Bug #67467: eval with parse error causes segmentation fault in generator
--FILE--
<?php

function gen() {
    $a = 1;
    yield $a;
}

@eval('abc');

$values = gen();
$values->next();

?>
===DONE===
--EXPECT--
===DONE===
