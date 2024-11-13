--TEST--
oss-fuzz #62294: Unsetting variable after ++/-- on string variable warning
--FILE--
<?php
set_error_handler(function($_, $m) {
    echo "$m\n";
    unset($GLOBALS['x']);
});

$x=" ";
echo "POST DEC\n";
var_dump($x--);

$x=" ";
echo "PRE DEC\n";
var_dump(--$x);

$x=" ";
echo "POST INC\n";
var_dump($x++);

$x=" ";
echo "PRE INC\n";
var_dump(++$x);
?>
--EXPECT--
POST DEC
Decrement on non-numeric string has no effect and is deprecated
string(1) " "
PRE DEC
Decrement on non-numeric string has no effect and is deprecated
string(1) " "
POST INC
Increment on non-alphanumeric string is deprecated
string(1) " "
PRE INC
Increment on non-alphanumeric string is deprecated
string(1) " "
