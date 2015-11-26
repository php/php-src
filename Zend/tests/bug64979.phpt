--TEST--
Bug #64979 (Wrong behavior of static variables in closure generators)
--FILE--
<?php

function new_closure_gen() {
    return function() {
        static $foo = 0;
        yield ++$foo;
    };
}

$closure1 = new_closure_gen();
$closure2 = new_closure_gen();

$gen1 = $closure1();
$gen2 = $closure1();
$gen3 = $closure2();

foreach (array($gen1, $gen2, $gen3) as $gen) {
    foreach ($gen as $val) {
        var_dump($val);
    }
}

?>
--EXPECT--
int(1)
int(2)
int(1)
