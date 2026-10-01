--TEST--
GH-22681: null bytes in name truncate ReflectionClass::__toString()
--FILE--
<?php

$obj = new class {};

$r = new ReflectionClass($obj);
echo $r;
var_dump( $r->getName() );

?>
--EXPECTF--
Class [ <user> class class@anonymous%0%s ] {
  @@ %s %d-%d

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}
string(%d) "class@anonymous%0%s"
