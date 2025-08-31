--TEST--
Bug #79600: Regression in 7.4.6 when yielding an array based generator
--FILE--
<?php

function createArrayGenerator() {
    yield from [
        1,
        2,
    ];
}

function createGeneratorFromArrayGenerator() {
    yield from createArrayGenerator();
}

foreach (createGeneratorFromArrayGenerator() as $value) {
    echo $value, "\n";
}

?>
--EXPECT--
1
2
