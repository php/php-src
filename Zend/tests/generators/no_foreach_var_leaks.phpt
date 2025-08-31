--TEST--
foreach() (and other) variables aren't leaked on premature close
--FILE--
<?php

function gen(array $array) {
    foreach ($array as $value) {
        yield $value;
    }
}

$gen = gen(['Foo', 'Bar']);
var_dump($gen->current());

// generator is closed here, without running SWITCH_FREE

?>
--EXPECT--
string(3) "Foo"
