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
Fatal error: Could not find trait Typo in %sbug63219.php on line %d
