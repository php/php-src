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
?>
--EXPECTF--
Warning: Variable of type integer does not accept array offsets in %s on line %d

Warning: Variable of type integer does not accept array offsets in %s on line %d

Deprecated: The each() function is deprecated. This message will be suppressed on further calls in %s on line %d
int(0)
string(3) "one"
int(1)
string(3) "two"
int(2)
int(3)

Warning: Variable of type boolean does not accept array offsets in %s on line %d

Warning: Variable of type boolean does not accept array offsets in %s on line %d
