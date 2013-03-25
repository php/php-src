--TEST--
025: Name ambiguity (class name & part of namespace name)
--FILE--
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

$x = new Foo;
Foo::Bar();
$x = new \Foo\Bar\Foo;
\Foo\Bar\Foo::Bar();
--EXPECT--
Foo\Bar\Foo
Foo\Bar\Foo
Foo\Bar\Foo
Foo\Bar\Foo
