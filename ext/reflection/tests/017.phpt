--TEST--
ReflectionClass::__toString() (constants)
--FILE--
<?php
class Foo {
    const test = "ok";
}
$class = new ReflectionClass("Foo");
echo $class;
?>
--EXPECTF--
Class [ <user> class Foo ] {
  @@ %s017.php 2-4

  - Constants [1] {
    Constant [ public string test ] { ok }
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
