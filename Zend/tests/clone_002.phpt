--TEST--
Testing multiple clone statements
--FILE--
<?php

$a = clone clone $b = new stdClass;
var_dump($a == $b);


$c = clone clone clone $b = new stdClass;
var_dump($a == $b, $b == $c);

class foo { }

$d = clone $a = $b = new foo;
try {
    var_dump($a == $d);
} catch (InvalidOperator) {
    echo "InvalidOperator thrown".PHP_EOL;
}
try {
    var_dump($b == $d);
} catch (InvalidOperator) {
    echo "InvalidOperator thrown".PHP_EOL;
}
try {
    var_dump($c == $a);
} catch (InvalidOperator) {
    echo "InvalidOperator thrown".PHP_EOL;
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
InvalidOperator thrown
InvalidOperator thrown
InvalidOperator thrown
