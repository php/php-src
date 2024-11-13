--TEST--
Unset property via error_handler
--FILE--
<?php
class C {
    public $a;

    public function errorHandler($errno, $errstr) {
        var_dump($errstr);
        unset($this->a);
    }
}

$c = new C;
set_error_handler([$c, 'errorHandler']);

/* default property value */
var_dump(--$c->a);

echo "NULL (only --)\n";
echo "POST DEC\n";
$c->a = null;
var_dump($c->a--);
unset($c->a);
echo "PRE DEC\n";
$c->a = null;
var_dump(--$c->a);
unset($c->a);
echo "Empty string\n";
echo "POST INC\n";
$c->a = "";
var_dump($c->a++);
unset($c->a);
echo "POST DEC\n";
$c->a = "";
var_dump($c->a--);
unset($c->a);
echo "PRE INC\n";
$c->a = "";
var_dump(++$c->a);
unset($c->a);
echo "PRE DEC\n";
$c->a = "";
var_dump(--$c->a);
unset($c->a);
echo "Non fill ASCII (only ++)\n";
echo "POST INC\n";
$c->a = " ad ";
var_dump($c->a++);
unset($c->a);
echo "PRE INC\n";
$c->a = " ad ";
var_dump(++$c->a);
unset($c->a);
echo "Bool\n";
echo "POST INC\n";
$c->a = false;
var_dump($c->a++);
unset($c->a);
echo "POST DEC\n";
$c->a = false;
var_dump($c->a--);
unset($c->a);
echo "PRE INC\n";
$c->a = false;
var_dump(++$c->a);
unset($c->a);
echo "PRE DEC\n";
$c->a = false;
var_dump(--$c->a);
unset($c->a);
echo "POST INC\n";
$c->a = true;
var_dump($c->a++);
unset($c->a);
echo "POST DEC\n";
$c->a = true;
var_dump($c->a--);
unset($c->a);
echo "PRE INC\n";
$c->a = true;
var_dump(++$c->a);
unset($c->a);
echo "PRE DEC\n";
$c->a = true;
var_dump(--$c->a);
unset($c->a);
?>
--EXPECT--
string(87) "Decrement on type null has no effect, this will change in the next major version of PHP"
NULL
NULL (only --)
POST DEC
string(87) "Decrement on type null has no effect, this will change in the next major version of PHP"
NULL
PRE DEC
string(87) "Decrement on type null has no effect, this will change in the next major version of PHP"
NULL
Empty string
POST INC
string(50) "Increment on non-alphanumeric string is deprecated"
string(0) ""
POST DEC
string(54) "Decrement on empty string is deprecated as non-numeric"
string(0) ""
PRE INC
string(50) "Increment on non-alphanumeric string is deprecated"
string(1) "1"
PRE DEC
string(54) "Decrement on empty string is deprecated as non-numeric"
int(-1)
Non fill ASCII (only ++)
POST INC
string(50) "Increment on non-alphanumeric string is deprecated"
string(4) " ad "
PRE INC
string(50) "Increment on non-alphanumeric string is deprecated"
string(4) " ad "
Bool
POST INC
string(87) "Increment on type bool has no effect, this will change in the next major version of PHP"
bool(false)
POST DEC
string(87) "Decrement on type bool has no effect, this will change in the next major version of PHP"
bool(false)
PRE INC
string(87) "Increment on type bool has no effect, this will change in the next major version of PHP"
bool(false)
PRE DEC
string(87) "Decrement on type bool has no effect, this will change in the next major version of PHP"
bool(false)
POST INC
string(87) "Increment on type bool has no effect, this will change in the next major version of PHP"
bool(true)
POST DEC
string(87) "Decrement on type bool has no effect, this will change in the next major version of PHP"
bool(true)
PRE INC
string(87) "Increment on type bool has no effect, this will change in the next major version of PHP"
bool(true)
PRE DEC
string(87) "Decrement on type bool has no effect, this will change in the next major version of PHP"
bool(true)
