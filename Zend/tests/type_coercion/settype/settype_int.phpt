--TEST--
casting different variables to integer using settype()
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
    settype($var, "int");
    var_dump($var);
}

echo "Done\n";
?>
--EXPECTF--
Deprecated: Implicit conversion from malformed string "string" to int is deprecated in %s on line %d
int(0)
int(8754456)

Deprecated: Implicit conversion from malformed string "" to int is deprecated in %s on line %d
int(0)

Deprecated: Implicit conversion from malformed string "%c" to int is deprecated in %s on line %d
int(0)
int(9876545)
int(0)
int(0)
int(1)
int(0)
int(1)

Deprecated: Implicit conversion from null to int is deprecated in %s on line %d
int(0)
int(%d)

Deprecated: Implicit conversion from test to int is deprecated in %s on line %d

Warning: Object of class test could not be converted to int in %s on line %d
int(1)
Done
