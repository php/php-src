--TEST--
Rebinding PFA of Closure rebinds inner Closure
--FILE--
<?php

class C {
    function getF() {
        return function ($a) { var_dump($this, $this === $a); };
    }
    function getG() {
        return static function ($a) { var_dump(static::class, $a); };
    }
}

class SubClass extends C {}

class Unrelated {}

$c = new C();
$f = $c->getF()(?);
$g = $c->getG()(?);

echo "# Can be rebound to \$this of the same class:\n";
$d = new C();
$f->bindTo($d)($d);

echo "# Can be rebound to \$this of a sub-class:\n";
$d = new SubClass();
$f->bindTo($d)($d);

echo "# Cannot be rebound to an unrelated class:\n";
try {
    $f->bindTo(new Unrelated)($c);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

echo "# Cannot unbind \$this on non-static closure:\n";
try {
    $f->bindTo(null)($c);
} catch (Error $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

echo "# Can unbind \$this on static closure:\n";
$g->bindTo(null)($c);

?>
--EXPECTF--
# Can be rebound to $this of the same class:
object(C)#%d (0) {
}
bool(true)
# Can be rebound to $this of a sub-class:
object(SubClass)#%d (0) {
}
bool(true)
# Cannot be rebound to an unrelated class:

Warning: Cannot bind method C::{closure:%s:%d}() to object of class Unrelated, this will be an error in PHP 9 in %s on line %d
Error: Value of type null is not callable
# Cannot unbind $this on non-static closure:

Warning: Cannot unbind $this of method, this will be an error in PHP 9 in %s on line %d
Error: Value of type null is not callable
# Can unbind $this on static closure:
string(1) "C"
object(C)#%d (0) {
}
