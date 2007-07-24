--TEST--
Bug #40757 (get_object_vars() get nothing in child class)
--FILE--
<?php
class Base {
  private $p1='sadf';

  function getFields($obj){
    return get_object_vars($obj);
  }
}

class Child extends Base { }

$base=new Base();
print_r($base->getFields(new Base()));
$child=new Child();
print_r($child->getFields(new Base()));
?>
--EXPECT--
Array
(
    [p1] => sadf
)
Array
(
    [p1] => sadf
)
