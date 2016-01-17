--TEST--
list() with non-integer-or-string keys
--FILE--
<?php

$results = [
    0 => 0,
    1 => 1,
    "" => ""
];

list(NULL => $NULL, 1.5 => $float, FALSE => $FALSE, TRUE => $TRUE) = $results;
var_dump($NULL, $float, $FALSE, $TRUE);

echo PHP_EOL;

list("0" => $zeroString, "1" => $oneString) = $results;
var_dump($zeroString, $oneString);

list(STDIN => $resource) = [];

?>
--EXPECTF--
string(0) ""
int(1)
int(0)
int(1)

int(0)
int(1)

Notice: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
