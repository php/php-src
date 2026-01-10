--TEST--
Abstract methods not allowed in classes that are not abstract (GH-16067)
--FILE--
<?php

// Still allowed via trait
trait TraitWithAbstract {
    abstract public function foo();
}
class TraitWorks {
    use TraitWithAbstract;
}

class NotAbstract {
    abstract public function bar();
}
?>
--EXPECTF--
Fatal error: Class NotAbstract declares abstract method bar() and must therefore be declared abstract in %s on line %d
