--TEST--
Pipes support return-by-reference
--FILE--
<?php

function &foo($val) {
    global $x;
    $x = $val;
    return $x;
}

function &bar() {
    return 42 |> foo(...);
}

$xRef = &bar();
$xRef++;
var_dump($x);

?>
--EXPECT--
int(43)
