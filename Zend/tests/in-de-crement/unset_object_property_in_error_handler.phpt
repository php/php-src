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
NULL
string(87) "Decrement on type null has no effect, this will change in the next major version of PHP"
NULL (only --)
POST DEC
NULL
string(87) "Decrement on type null has no effect, this will change in the next major version of PHP"
PRE DEC
NULL
string(87) "Decrement on type null has no effect, this will change in the next major version of PHP"
Empty string
POST INC
string(0) ""
string(74) "Increment on non-numeric string is deprecated, use str_increment() instead"
POST DEC
string(0) ""
string(54) "Decrement on empty string is deprecated as non-numeric"
PRE INC
string(1) "1"
string(74) "Increment on non-numeric string is deprecated, use str_increment() instead"
PRE DEC
int(-1)
string(54) "Decrement on empty string is deprecated as non-numeric"
Non fill ASCII (only ++)
POST INC
string(4) " ad "
string(74) "Increment on non-numeric string is deprecated, use str_increment() instead"
PRE INC
string(4) " ad "
string(74) "Increment on non-numeric string is deprecated, use str_increment() instead"
Bool
POST INC
bool(false)
string(87) "Increment on type bool has no effect, this will change in the next major version of PHP"
POST DEC
bool(false)
string(87) "Decrement on type bool has no effect, this will change in the next major version of PHP"
PRE INC
bool(false)
string(87) "Increment on type bool has no effect, this will change in the next major version of PHP"
PRE DEC
bool(false)
string(87) "Decrement on type bool has no effect, this will change in the next major version of PHP"
POST INC
bool(true)
string(87) "Increment on type bool has no effect, this will change in the next major version of PHP"
POST DEC
bool(true)
string(87) "Decrement on type bool has no effect, this will change in the next major version of PHP"
PRE INC
bool(true)
string(87) "Increment on type bool has no effect, this will change in the next major version of PHP"
PRE DEC
bool(true)
string(87) "Decrement on type bool has no effect, this will change in the next major version of PHP"
