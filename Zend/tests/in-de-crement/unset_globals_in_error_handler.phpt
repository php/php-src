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
Decrement on type null has no effect, this will change in the next major version of PHP
NULL
PRE DEC
Decrement on type null has no effect, this will change in the next major version of PHP
NULL
Empty string
POST INC
Increment on non-alphanumeric string is deprecated
string(0) ""
POST DEC
Decrement on empty string is deprecated as non-numeric
string(0) ""
PRE INC
Increment on non-alphanumeric string is deprecated
string(1) "1"
PRE DEC
Decrement on empty string is deprecated as non-numeric
int(-1)
Non fill ASCII (only ++)
POST INC
Increment on non-alphanumeric string is deprecated
string(4) " ad "
PRE INC
Increment on non-alphanumeric string is deprecated
string(4) " ad "
Bool
POST INC
Increment on type bool has no effect, this will change in the next major version of PHP
bool(false)
POST DEC
Decrement on type bool has no effect, this will change in the next major version of PHP
bool(false)
PRE INC
Increment on type bool has no effect, this will change in the next major version of PHP
bool(false)
PRE DEC
Decrement on type bool has no effect, this will change in the next major version of PHP
bool(false)
POST INC
Increment on type bool has no effect, this will change in the next major version of PHP
bool(true)
POST DEC
Decrement on type bool has no effect, this will change in the next major version of PHP
bool(true)
PRE INC
Increment on type bool has no effect, this will change in the next major version of PHP
bool(true)
PRE DEC
Decrement on type bool has no effect, this will change in the next major version of PHP
bool(true)
