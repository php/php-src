--TEST--
Explicit (bool) cast must not warn
--FILE--
<?php

$values =[
    3.0,
    3.5,
    "hello",
    "false",
    5,
    99,
    -33,
];

foreach($values as $value) {
    var_dump((bool) $value);
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
