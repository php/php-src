--TEST--
SPL: spl_autoload_register() Bug #48541: registering multiple closures fails with memleaks
--FILE--
<?php

class X {
  public function getClosure() {
    return function($class) {
      echo "a2 called\n";
    };
  }
}

$a = function ($class) {
    echo "a called\n";
};
$x = new X;
$a2 = $x->getClosure();
$b = function ($class) {
    eval('class ' . $class . '{function __construct(){echo "foo\n";}}');
    echo "b called\n";
};
spl_autoload_register($a);
spl_autoload_register($a2);
spl_autoload_register($b);

$c = $a;
$c2 = $a2;
spl_autoload_register($c);
spl_autoload_register($c2);
$c = new foo;
?>
===DONE===
--EXPECT--
a called
a2 called
b called
foo
===DONE===