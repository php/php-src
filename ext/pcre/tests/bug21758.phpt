--TEST--
Bug #27011: preg_replace_callback() not working with class methods
--FILE--
<?php
  class Foo {
    function foo() {
      
      $s = 'preg_replace() is broken';
      
      var_dump(preg_replace_callback(
              '/broken/',
              array(&$this, 'bar'),
              $s
           ));
    }
    
    function bar() {
      return 'working';
    }
    
  } // of Foo

  $o = new Foo;
?>
--EXPECT--
string(25) "preg_replace() is working"
--UEXPECT--
unicode(25) "preg_replace() is working"
