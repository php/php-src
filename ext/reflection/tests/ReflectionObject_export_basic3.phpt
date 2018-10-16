--TEST--
ReflectionObject::export() - ensure dynamic property with same name as inherited private property is shown.
--FILE--
<?php
class C {
	private $p = 1;
}

class D extends C{
}

$Obj = new D;
$Obj->p = 'value';
ReflectionObject::export($Obj)
?>
--EXPECTF--
Object of class [ <user> class D extends C ] {
  @@ %s 6-7

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Dynamic properties [0] {
  }

  - Methods [0] {
  }
}
