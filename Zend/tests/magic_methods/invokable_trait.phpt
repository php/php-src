--TEST--
Invokable is implicitly declared when __invoke() comes from a trait
--FILE--
<?php

trait T {
    public function __invoke(): string {
        return "ok";
    }
}

/* Nested trait */
trait T2 {
    use T;
}

class C {
    use T;
}
class C2 {
    use T2;
}

var_dump(new C instanceof Invokable);
var_dump(new C2 instanceof Invokable);

/* The traits themselves should not implement Invokable */
$rc = new ReflectionClass(T::class);
var_dump($rc->getInterfaceNames());
$rc = new ReflectionClass(T2::class);
var_dump($rc->getInterfaceNames());

/* But the classes using the traits should have Invokable */
$rc = new ReflectionClass(C::class);
var_dump($rc->getInterfaceNames());
$rc = new ReflectionClass(C2::class);
var_dump($rc->getInterfaceNames());

/* Class that already has __invoke and uses a trait with __invoke */
class CWithOwn {
    use T;
    public function __invoke(): string {
        return "own";
    }
}

var_dump(new CWithOwn instanceof Invokable);

?>
--EXPECT--
bool(true)
bool(true)
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  string(9) "Invokable"
}
array(1) {
  [0]=>
  string(9) "Invokable"
}
bool(true)
