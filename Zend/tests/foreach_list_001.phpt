--TEST--
foreach with list syntax
--FILE--
<?php

foreach(array(array(1,2), array(3,4)) as list($a, $b)) {
    var_dump($a . $b);
}

$array = array(
    array('a', 'b'),
    array('c', 'd'),
);

foreach ($array as list($a, $b)) {
    var_dump($a . $b);
}


$multi = array(
    array(array(1,2), array(3,4)),
    array(array(5,6), array(7,8)),
);

foreach ($multi as list(list($a, $b), list($c, $d))) {
    var_dump($a . $b . $c . $d);
}

foreach ($multi as $key => list(list($a, $b), list($c, $d))) {
    var_dump($key . $a . $b . $c . $d);
}


?>
--EXPECT--
string(2) "12"
string(2) "34"
string(2) "ab"
string(2) "cd"
string(4) "1234"
string(4) "5678"
string(5) "01234"
string(5) "15678"
