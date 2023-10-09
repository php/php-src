--TEST--
Error handler can change type of operand of ++ to object with do_operator
--EXTENSIONS--
gmp
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    var_dump($errstr);
    global $x;
    $x = gmp_init(10);
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
string(50) "Increment on non-alphanumeric string is deprecated"
string(4) "foo!"
string(50) "Increment on non-alphanumeric string is deprecated"
string(1) "!"
DONE
