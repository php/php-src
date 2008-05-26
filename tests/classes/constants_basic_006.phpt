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
unicode(5) "hello"
array(2) {
  [u"nasty"]=>
  unicode(4) "test"
  [u"hello"]=>
  unicode(5) "nasty"
}
unicode(5) "hello"
array(2) {
  [u"nasty"]=>
  unicode(4) "test"
  [u"hello"]=>
  unicode(5) "nasty"
}
unicode(5) "hello"
array(2) {
  [u"nasty"]=>
  unicode(4) "test"
  [u"hello"]=>
  unicode(5) "nasty"
}
