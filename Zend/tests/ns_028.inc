<?php
namespace Foo;

class Foo {
  function __construct() {
  	echo "Method - ".__CLASS__."::".__FUNCTION__."\n";
  }
  static function Bar() {
  	echo "Method - ".__CLASS__."::".__FUNCTION__."\n";
  }
}

function Bar() {
  echo "Func   - ".__FUNCTION__."\n";
}
