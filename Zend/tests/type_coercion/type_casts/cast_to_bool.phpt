--TEST--
casting different variables to boolean
--FILE--
<?php

$r = fopen(__FILE__, "r");

class test {
    function  __toString() {
        return "10";
    }
}

$o = new test;

$vars = array(
    "string",
    "8754456",
    "",
    "\0",
    9876545,
    0.10,
    array(),
    array(1,2,3),
    false,
    true,
    NULL,
    $r,
    $o
);

foreach ($vars as $var) {
    $tmp = (bool)$var;
    var_dump($tmp);
}

echo "Done\n";
?>
--EXPECTF--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)

Deprecated: Implicit conversion from null to bool is deprecated in %s on line %d
bool(false)
bool(true)

Deprecated: Implicit conversion from test to bool is deprecated in %s on line %d
bool(true)
Done
