--TEST--
Bug #62892 (ReflectionClass::getTraitAliases crashes on importing trait methods as private)
--FILE--
<?php

trait myTrait {
     public function run() {}
}

class myClass {
     use myTrait {
         MyTrait::run as private;
     }
}
$class = new \ReflectionClass('myClass');
var_dump($class->getTraitAliases());

?>
--EXPECT--
array(0) {
}
