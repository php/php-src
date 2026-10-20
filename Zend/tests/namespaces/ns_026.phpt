--TEST--
026: Name ambiguity (class name & namespace name)
--INI--
opcache.optimization_level=0
--FILE--
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

$x = new Foo;
\Foo\Bar();
$x = new \Foo\Foo;
\Foo\Foo::Bar();
\Foo\Bar();
Foo\Bar();
?>
--EXPECTF--
Method - Foo\Foo::__construct
Func   - Foo\Bar
Method - Foo\Foo::__construct
Method - Foo\Foo::Bar
Func   - Foo\Bar

Fatal error: Uncaught Error: Call to undefined function Foo\Foo\Bar() in %sns_026.php:%d
Stack trace:
#0 {main}
  thrown in %sns_026.php on line %d
