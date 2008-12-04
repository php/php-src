<?php
namespace Foo\Bar;

class Foo {
  function __construct() {
  	echo __CLASS__,"\n";
  }
  static function Bar() {
  	echo __CLASS__,"\n";
  }
}
