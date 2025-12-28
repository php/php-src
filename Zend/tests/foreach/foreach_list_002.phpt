--TEST--
foreach with freak lists
--FILE--
<?php

foreach (array(array(1,2), array(3,4)) as list($a, )) {
    var_dump($a);
}

echo "Array of strings:\n";
$array = [['a', 'b'], 'c', 'd'];

foreach($array as list(, $a)) {
   var_dump($a);
}

echo "Array of ints:\n";
$array = [[5, 6], 10, 20];

foreach($array as list(, $a)) {
   var_dump($a);
}

echo "Array of nulls:\n";
$array = [[null, null], null, null];

foreach($array as list(, $a)) {
   var_dump($a);
}

?>
--EXPECTF--
int(1)
int(3)
Array of strings:
string(1) "b"

Warning: Cannot use string as array in %s on line %d
NULL

Warning: Cannot use string as array in %s on line %d
NULL
Array of ints:
int(6)

Warning: Cannot use int as array in %s on line %d
NULL

Warning: Cannot use int as array in %s on line %d
NULL
Array of nulls:
NULL
NULL
NULL
