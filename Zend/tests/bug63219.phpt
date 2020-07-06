--TEST--
Bug #63219 (Segfault when aliasing trait method when autoloader throws excpetion)
--FILE--
<?php
trait TFoo {
    public function fooMethod(){}
}

class C {
    use TFoo {
        Typo::fooMethod as tf;
    }
}

echo "okey";
?>
--EXPECTF--
Fatal error: Trait Typo cannot be found in %s on line %d
