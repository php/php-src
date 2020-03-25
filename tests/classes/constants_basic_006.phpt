--TEST--
Ensure class constants are not evaluated when a class is looked up to resolve inheritance during runtime.
--FILE--
<?php
  class C
  {
      const X = E::A;
      public static $a = array(K => D::V, E::A => K);
  }

  eval('class D extends C { const V = \'test\'; }');

  class E extends D
  {
      const A = "hello";
  }

  define('K', "nasty");

  var_dump(C::X, C::$a, D::X, D::$a, E::X, E::$a);
?>
--EXPECT--
string(5) "hello"
array(2) {
  ["nasty"]=>
  string(4) "test"
  ["hello"]=>
  string(5) "nasty"
}
string(5) "hello"
array(2) {
  ["nasty"]=>
  string(4) "test"
  ["hello"]=>
  string(5) "nasty"
}
string(5) "hello"
array(2) {
  ["nasty"]=>
  string(4) "test"
  ["hello"]=>
  string(5) "nasty"
}
