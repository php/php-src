--TEST--
GH-11178 (Segmentation fault in spl_array_it_get_current_data (PHP 8.1.18))
--FILE--
<?php
#[AllowDynamicProperties]
class A implements IteratorAggregate {
    function __construct() {
        $this->{'x'} = 1;
    }

    function getIterator(): Traversable {
        return new ArrayIterator($this);
    }
}

$obj = new A;

foreach ($obj as $k => &$v) {
    $v = 3;
}

var_dump($obj);
?>
--EXPECT--
object(A)#1 (1) {
  ["x"]=>
  &int(3)
}
