--TEST--
Error handler can change type of operand of ++
--FILE--
<?php

set_error_handler(function () {
    global $x;
    $x = 1;
});

$x = '';
$x++;
var_dump($x);

set_error_handler(function ($errno, $errstr) {
    var_dump($errstr);
    global $x;
    $x = new stdClass;
});

$x = 'foo!';
$x++;
var_dump($x);

/* Interned string */
$x = '!';
$x++;
var_dump($x);

?>
DONE
--EXPECT--
string(1) "1"
string(50) "Increment on non-alphanumeric string is deprecated"
string(4) "foo!"
string(50) "Increment on non-alphanumeric string is deprecated"
string(1) "!"
DONE
