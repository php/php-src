--TEST--
Test properties with array default values using class constants as keys and values.
--FILE--
<?php
  class X
  {
      // Static and instance array using class constants
      public static $sa_x = array(B::KEY => B::VALUE);
      public $a_x = array(B::KEY => B::VALUE);
  }

  class B
  {
      const KEY = "key";
      const VALUE = "value";

      // Static and instance array using class constants with self
      public static $sa_b = array(self::KEY => self::VALUE);
      public $a_b = array(self::KEY => self::VALUE);
  }

  class C extends B
  {
      // Static and instance array using class constants with parent
      public static $sa_c_parent = array(parent::KEY => parent::VALUE);
      public $a_c_parent = array(parent::KEY => parent::VALUE);

      // Static and instance array using class constants with self (constants should be inherited)
      public static $sa_c_self = array(self::KEY => self::VALUE);
      public $a_c_self = array(self::KEY => self::VALUE);

      // Should also include inherited properties from B.
  }

  echo "\nStatic properties:\n";
  var_dump(X::$sa_x, B::$sa_b, C::$sa_b, C::$sa_c_parent, C::$sa_c_self);

  echo "\nInstance properties:\n";
  $x = new x;
  $b = new B;
  $c = new C;
  var_dump($x, $b, $c);
?>
--EXPECTF--
Static properties:
array(1) {
  ["key"]=>
  string(5) "value"
}
array(1) {
  ["key"]=>
  string(5) "value"
}
array(1) {
  ["key"]=>
  string(5) "value"
}
array(1) {
  ["key"]=>
  string(5) "value"
}
array(1) {
  ["key"]=>
  string(5) "value"
}

Instance properties:
object(X)#%d (1) {
  ["a_x"]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
}
object(B)#%d (1) {
  ["a_b"]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
}
object(C)#%d (3) {
  ["a_c_parent"]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
  ["a_c_self"]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
  ["a_b"]=>
  array(1) {
    ["key"]=>
    string(5) "value"
  }
}
