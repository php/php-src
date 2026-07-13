--TEST--
GH-22681: null bytes in name truncate ReflectionProperty::__toString()
--FILE--
<?php

$obj = (object)["F\0oo" => true];
$r = new ReflectionProperty($obj, "F\0oo");
echo $r;
var_dump( $r->getDocComment() );

echo new ReflectionObject($obj);

?>
--EXPECTF--
Property [ <dynamic> public $F%0oo ]
bool(false)
Object of class [ <internal:Core> class stdClass ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Dynamic properties [1] {
    Property [ <dynamic> public $F%0oo ]
  }

  - Methods [0] {
  }
}
