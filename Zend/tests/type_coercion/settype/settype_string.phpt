--TEST--
casting different variables to string using settype()
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
    settype($var, "string");
    var_dump($var);
}

echo "Done\n";
?>
--EXPECTF--
string(6) "string"
string(7) "8754456"
string(0) ""
string(1) "%0"
string(7) "9876545"
string(3) "0.1"

Warning: Array to string conversion in %s on line %d
string(5) "Array"

Warning: Array to string conversion in %s on line %d
string(5) "Array"
string(0) ""
string(1) "1"
string(0) ""
string(%d) "Resource id #%d"
string(2) "10"
Done
