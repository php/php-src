--TEST--
testing hash returns array
--FILE--
<?php
class Foo {
    public function __hash() {
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


