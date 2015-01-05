--TEST--
Generator keys are auto-incrementing by default
--FILE--
<?php

function gen() {
    yield 'foo';
    yield 'bar';
    yield 5 => 'rab';
    yield 'oof';
}

foreach (gen() as $k => $v) {
    echo $k, ' => ', $v, "\n";
}

?>
--EXPECT--
0 => foo
1 => bar
5 => rab
6 => oof
