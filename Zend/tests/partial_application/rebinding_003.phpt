--TEST--
Rebinding PFA of Closure rebinds inner Closure
--FILE--
<?php

class C {
    function f($a) { var_dump($this, $this === $a); }
    static function g($a) { var_dump(static::class); }
}

class SubClass extends C {}

class Unrelated {}

$c = new C();
$f = $c->f(?);
$g = $c->g(?);

echo "# Can be rebound to \$this of the same class:\n";
$f->bindTo(new C)($c);

echo "# Can be rebound to \$this of a sub-class:\n";
$f->bindTo(new SubClass)($c);

echo "# Cannot be rebound to an unrelated class:\n";
try {
    $f->bindTo(new Unrelated)($c);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "# Cannot unbind \$this on instance method:\n";
try {
    $f->bindTo(null)($c);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

echo "# Can unbind \$this on static method:\n";
try {
    $g->bindTo(null)($c);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
# Can be rebound to $this of the same class:
object(C)#%d (0) {
}
bool(false)
# Can be rebound to $this of a sub-class:
object(SubClass)#%d (0) {
}
bool(false)
# Cannot be rebound to an unrelated class:

Warning: Cannot bind method C::{closure:%s:%d}() to object of class Unrelated, this will be an error in PHP 9 in %s on line %d
Value of type null is not callable
# Cannot unbind $this on instance method:

Warning: Cannot unbind $this of method, this will be an error in PHP 9 in %s on line %d
Value of type null is not callable
# Can unbind $this on static method:
string(1) "C"
