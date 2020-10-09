--TEST--
Bug #45571 (ReflectionClass::__toString() shows superclasses' private static methods.)
--FILE--
<?php

Class A {
    static private $a 	= 0;
    static protected $b = 1;
    static public $c 	= 2;

    private function f() {}
    private static function sf() {}
}

Class C extends A { }

echo new ReflectionClass("C");

?>
--EXPECTF--
Class [ <user> class C extends A ] {
  @@ %s 12-12

  - Constants [0] {
  }

  - Static properties [2] {
    Property [ protected static $b = 1 ]
    Property [ public static $c = 2 ]
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}
