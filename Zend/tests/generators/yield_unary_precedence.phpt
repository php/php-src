--TEST--
When + or - are used on yield, they must be unary (and not binary) (Bug #69160)
--FILE--
<?php
function gen() {
    var_dump(yield +1);
    var_dump(yield -1);
    var_dump(yield * -1); // other ops still should behave normally
}

for ($gen = gen(); $gen->valid(); $gen->send(1)) {
    echo "\n";
    var_dump($gen->current());
}
?>
--EXPECT--
int(1)
int(1)

int(-1)
int(1)

NULL
int(-1)
