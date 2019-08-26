--TEST--
Object properties HT may need to be removed from nested data
--FILE--
<?php

class Test {
    public function __destruct() {
        $GLOBALS['x'] = $this;
    }
}

$t = new Test;
$t->x = new stdClass;
$t->x->t = $t;
$a = (array) $t->x;
unset($t, $a);
gc_collect_cycles();
var_dump($x);

// TODO: The destructor *should* be running here, but doesn't.
// This works in PHP >= 7.3 though.

?>
--EXPECTF--
Notice: Undefined variable: x in %s on line %d
NULL
