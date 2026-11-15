--TEST--
Reading a leading-numeric string offset when the error handler releases the string
--FILE--
<?php
set_error_handler(function ($errno, $errstr) {
    $GLOBALS['s'] = null;
    echo $errstr, "\n";
});

function make(int $n) {
    return str_repeat("abc", $n);
}

function test() {
    global $s;
    return $s["1x"];
}

$s = make(2);
test();
var_dump($s);
?>
--EXPECT--
Illegal string offset "1x"
NULL
