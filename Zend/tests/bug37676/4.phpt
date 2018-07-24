--TEST--
Bug #37676 (List and Loop warnings)
--FILE--
<?php
$a = array('one', 'two', 3);

$a[0][1]; // Valid

$a[2][0]; // Integer Error

list($j, $k, list($l)) = $a; // Integer Error

while (list($k, $v) = each($a)) { // each dep, boolean err
    var_dump($k, $v);
}

class _test {
    function __construct() {
        $this->elem = [
            [0, 'one'],
            [1, 2]
        ];
    }

    function pop() {
        if (count($this->elem)) {
          return array_shift($this->elem);
        }
        return false;
    }
}

$t1 = new _test();
while (list($k, $v) = $t1->pop()) {
    var_dump($k, $v);
}

$t2 = new _test();
while (@list($k, $v) = $t2->pop()) {
    var_dump($k, $v);
}
?>
--EXPECTF--
Warning: Variable of type int does not accept array offsets in %s on line %d

Warning: Variable of type int does not accept array offsets in %s on line %d

Deprecated: The each() function is deprecated. This message will be suppressed on further calls in %s on line %d
int(0)
string(3) "one"
int(1)
string(3) "two"
int(2)
int(3)

Warning: Variable of type bool does not accept array offsets in %s on line %d

Warning: Variable of type bool does not accept array offsets in %s on line %d
int(0)
string(3) "one"
int(1)
int(2)

Warning: Variable of type bool does not accept array offsets in %s on line %d

Warning: Variable of type bool does not accept array offsets in %s on line %d
int(0)
string(3) "one"
int(1)
int(2)
