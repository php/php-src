--TEST--
Attempting to access static properties using instance property syntax 
--FILE--
<?php
class C {
    public static $x = 'C::$x';
    protected static $y = 'C::$y';
}

$c = new C;

echo "\n--> Access visible static prop like instance prop:\n";
var_dump(isset($c->x));
unset($c->x);
echo $c->x;
$c->x = 1;
$ref = 'ref';
$c->x =& $ref;
var_dump($c->x, C::$x);

echo "\n--> Access non-visible static prop like instance prop:\n";
var_dump(isset($c->y));
//unset($c->y);		// Fatal error, tested in static_properties_003_error1.phpt
//echo $c->y;		// Fatal error, tested in static_properties_003_error2.phpt
//$c->y = 1;		// Fatal error, tested in static_properties_003_error3.phpt
//$c->y =& $ref;	// Fatal error, tested in static_properties_003_error4.phpt
?>
==Done==
--EXPECTF--
--> Access visible static prop like instance prop:
bool(false)

Strict Standards: Accessing static property C::$x as non static in %s on line 11

Strict Standards: Accessing static property C::$x as non static in %s on line 12

Notice: Undefined property: C::$x in %s on line 12

Strict Standards: Accessing static property C::$x as non static in %s on line 13

Strict Standards: Accessing static property C::$x as non static in %s on line 15

Strict Standards: Accessing static property C::$x as non static in %s on line 16
%unicode|string%(3) "ref"
%unicode|string%(5) "C::$x"

--> Access non-visible static prop like instance prop:
bool(false)
==Done==
