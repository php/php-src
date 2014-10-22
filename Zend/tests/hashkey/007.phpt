--TEST--
testing hashKey returns array
--FILE--
<?php
class Foo {
    public function __hashKey() {
        return [];
    }
}

$foo = new Foo();
$test = [
    $foo => true
];

var_dump($test);
?>
--EXPECTF--	
Warning: Illegal offset type in %s on line %d
array(0) {
}


