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
Fatal error: Class NotAbstract contains 1 abstract method and must therefore be declared abstract or implement the remaining method (NotAbstract::bar) in %s on line %d
