--TEST--
testing hash returns resource
--FILE--
<?php
class Foo {
    public function __hash() {
        return fopen("php://stdin", "r");
    }
}

$foo = new Foo();
$test = [
    $foo => true
];

var_dump($test);
?>
--EXPECTF--	
Warning: Illegal offset type in %s on line 11
array(0) {
}


