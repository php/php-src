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
string(74) "Increment on non-numeric string is deprecated, use str_increment() instead"
string(4) "foo!"
string(74) "Increment on non-numeric string is deprecated, use str_increment() instead"
string(1) "!"
DONE
