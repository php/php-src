--TEST--
027: Name ambiguity (class name & part of extertnal namespace name)
--FILE--
<?php
require "ns_027.inc";

class Foo {
  function __construct() {
  	echo __CLASS__,"\n";
  }
  static function Bar() {
  	echo __CLASS__,"\n";
  }
}

$x = new Foo;
Foo::Bar();
$x = new Foo\Bar\Foo;
Foo\Bar\Foo::Bar();
--EXPECT--
Foo
Foo
Foo\Bar\Foo
Foo\Bar\Foo
