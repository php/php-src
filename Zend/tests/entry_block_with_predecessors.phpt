--TEST--
For SSA form the entry block should have no predecessors
--FILE--
<?php

function test() {
    while (true) {
        var_dump($a + 1);
        $a = 1;
        if (@$i++) {
            break;
        }
    }
}

function test2() {
    while (true) {
        $a = 42;
        if (@$i++ > 1) {
            break;
        }
        $a = new stdClass;
    }
}

test();
test2();

?>
--EXPECTF--
Warning: Undefined variable $a in %s on line %d
int(1)
int(2)
