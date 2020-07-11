--TEST--
Bug #21758 (preg_replace_callback() not working with class methods)
--FILE--
<?php
  class Foo {
    function __construct() {

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
