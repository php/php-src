--TEST--
Bug GH-9285: PHP 8.2 readonly classes allow inheriting mutable properties from traits
--FILE--
<?php

trait T2 {
    public $prop;
}

readonly class G {
    use T2;
}

?>
--EXPECTF--
Fatal error: Cannot use traits inside of readonly classes. T2 is used in G in %s on line %d
