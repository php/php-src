--TEST--
SplFixedArray - values should be gced after var_export then being modified
--FILE--
<?php
class HasDestructor {
    public function __destruct() {
        echo "In destructor\n";
    }
}
$array = SplFixedArray::fromArray([new HasDestructor()]);
var_dump($array);
echo "Replacing\n";
$array[0] = new stdClass();
// As of php 8.3, this will be freed before the var_dump call
echo "Dumping again\n";
var_dump($array);
// As of php 8.3, this only contain object properties (dynamic properties and declared subclass properties)
var_dump(get_mangled_object_vars($array));
?>
--EXPECT--
object(SplFixedArray)#2 (1) {
  [0]=>
  object(HasDestructor)#1 (0) {
  }
}
Replacing
In destructor
Dumping again
object(SplFixedArray)#2 (1) {
  [0]=>
  object(stdClass)#3 (0) {
  }
}
array(0) {
}
