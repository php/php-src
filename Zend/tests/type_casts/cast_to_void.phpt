--TEST--
casting different variables to void
--FILE--
<?php

$r = fopen(__FILE__, "r");

class test {
    private $var1 = 1;
    public $var2 = 2;
    protected $var3 = 3;

    function  __toString() {
        return "10";
    }
}

$o = new test;

$vars = array(
    "string",
    "",
    "\0",
    "8754456",
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
    (void)$var;
}

// Cast literals to verify behavior for optimized const zvals
(void)"foo";
(void)["foo", "bar"];

echo "Done\n";
?>
--EXPECTF--
Done
