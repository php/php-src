--TEST--
Bug #81582: Falsifiable not implicitly declared if __toBool() came from a trait
--FILE--
<?php

trait T {
    public function __toBool(): bool {
        return true;
    }
}
trait T2 {
    use T;
}

class C {
    use T;
}
class C2 {
    use T2;
}

var_dump(new C instanceof Falsifiable);
var_dump(new C2 instanceof Falsifiable);

// The traits themselves should not implement Falsifiable -- traits cannot implement interfaces.
$rc = new ReflectionClass(T::class);
var_dump($rc->getInterfaceNames());
$rc = new ReflectionClass(T2::class);
var_dump($rc->getInterfaceNames());

?>
--EXPECT--
bool(true)
bool(true)
array(0) {
}
array(0) {
}
