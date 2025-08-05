--TEST--
PFA scope cannot be rebound
--FILE--
<?php

class C {
    function f($a) { var_dump(static::class); }
}

class SubClass extends C {}

function g($a) { var_dump(__FUNCTION__); }

$c = new C;
$f = $c->f(?);
$g = g(?);

echo "# Can be rebound to the same scope:\n";
$f->bindTo($c, C::class)(1);

echo "# Method cannot be rebound to a different scope:\n";
try {
    $f->bindTo($c, SubClass::class)(1);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "# Function cannot be refound to a different scope:\n";
try {
    $g->bindTo($c, SubClass::class)(1);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
# Can be rebound to the same scope:
string(1) "C"
# Method cannot be rebound to a different scope:

Warning: Cannot rebind scope of closure created from method, this will be an error in PHP 9 in %s on line %d
Value of type null is not callable
# Function cannot be refound to a different scope:

Warning: Cannot bind an instance to a static closure, this will be an error in PHP 9 in %s on line %d
Value of type null is not callable
