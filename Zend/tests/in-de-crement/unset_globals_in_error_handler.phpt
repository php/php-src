--TEST--
Unset variable via $GLOBALS array in error_handler
--FILE--
<?php
set_error_handler(function($_, $m) {
    echo "$m\n";
    unset($GLOBALS['x']);
});
echo "NULL (only --)\n";
echo "POST DEC\n";
$x = null;
var_dump($x--);
unset($x);
echo "PRE DEC\n";
$x = null;
var_dump(--$x);
unset($x);
echo "Empty string\n";
echo "POST INC\n";
$x = "";
var_dump($x++);
unset($x);
echo "POST DEC\n";
$x = "";
var_dump($x--);
unset($x);
echo "PRE INC\n";
$x = "";
var_dump(++$x);
unset($x);
echo "PRE DEC\n";
$x = "";
var_dump(--$x);
unset($x);
echo "Non fill ASCII (only ++)\n";
echo "POST INC\n";
$x = " ad ";
var_dump($x++);
unset($x);
echo "PRE INC\n";
$x = " ad ";
var_dump(++$x);
unset($x);
echo "Bool\n";
echo "POST INC\n";
$x = false;
var_dump($x++);
unset($x);
echo "POST DEC\n";
$x = false;
var_dump($x--);
unset($x);
echo "PRE INC\n";
$x = false;
var_dump(++$x);
unset($x);
echo "PRE DEC\n";
$x = false;
var_dump(--$x);
unset($x);
echo "POST INC\n";
$x = true;
var_dump($x++);
unset($x);
echo "POST DEC\n";
$x = true;
var_dump($x--);
unset($x);
echo "PRE INC\n";
$x = true;
var_dump(++$x);
unset($x);
echo "PRE DEC\n";
$x = true;
var_dump(--$x);
unset($x);
?>
--EXPECT--
NULL (only --)
POST DEC
NULL
Decrement on type null has no effect, this will change in the next major version of PHP
PRE DEC
NULL
Decrement on type null has no effect, this will change in the next major version of PHP
Empty string
POST INC
string(0) ""
Increment on non-numeric string is deprecated, use str_increment() instead
POST DEC
string(0) ""
Decrement on empty string is deprecated as non-numeric
PRE INC
string(1) "1"
Increment on non-numeric string is deprecated, use str_increment() instead
PRE DEC
int(-1)
Decrement on empty string is deprecated as non-numeric
Non fill ASCII (only ++)
POST INC
string(4) " ad "
Increment on non-numeric string is deprecated, use str_increment() instead
PRE INC
string(4) " ad "
Increment on non-numeric string is deprecated, use str_increment() instead
Bool
POST INC
bool(false)
Increment on type bool has no effect, this will change in the next major version of PHP
POST DEC
bool(false)
Decrement on type bool has no effect, this will change in the next major version of PHP
PRE INC
bool(false)
Increment on type bool has no effect, this will change in the next major version of PHP
PRE DEC
bool(false)
Decrement on type bool has no effect, this will change in the next major version of PHP
POST INC
bool(true)
Increment on type bool has no effect, this will change in the next major version of PHP
POST DEC
bool(true)
Decrement on type bool has no effect, this will change in the next major version of PHP
PRE INC
bool(true)
Increment on type bool has no effect, this will change in the next major version of PHP
PRE DEC
bool(true)
Decrement on type bool has no effect, this will change in the next major version of PHP
