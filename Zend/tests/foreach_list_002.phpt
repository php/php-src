--TEST--
foreach with freak lists
--FILE--
<?php

foreach (array(array(1,2), array(3,4)) as list($a, )) {
    var_dump($a);
}

$array = [['a', 'b'], 'c', 'd'];

foreach($array as list(, $a)) {
   var_dump($a);
}

?>
--EXPECTF--
int(1)
int(3)
string(1) "b"
NULL
NULL
