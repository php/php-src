--TEST--
Trait property attribute compatibility validation
--FILE--
<?php

trait T1 {
    public $prop1;
}

trait T2 {
    public $prop1;
}

class C {
    use T1, T2;
}

?>
===DONE===
--EXPECT--
===DONE===
