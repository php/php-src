--TEST--
casting to void is a statement
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
    $tmp = (void)$var;
    var_dump($tmp);
}

echo "Done\n";
?>
--EXPECTF--
Parse error: syntax error, unexpected token "(void)" in %s on line %d
