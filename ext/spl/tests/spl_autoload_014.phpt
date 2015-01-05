--TEST--
SPL: spl_autoload_unregister() with closures and invokables
--FILE--
<?php
$closure = function($class) {
  echo "closure called with class $class\n";
};

class Autoloader {
  private $dir;
  public function __construct($dir) {
    $this->dir = $dir;
  }
  public function __invoke($class) {
    echo ("Autoloader('{$this->dir}') called with $class\n");
  }
}

class WorkingAutoloader {
  public function __invoke($class) {
    echo ("WorkingAutoloader() called with $class\n");
    eval("class $class { }");
  }
}

$al1 = new Autoloader('d1');
$al2 = new WorkingAutoloader('d2');

spl_autoload_register($closure);
spl_autoload_register($al1);
spl_autoload_register($al2);

$x = new TestX;

spl_autoload_unregister($closure);
spl_autoload_unregister($al1);

$y = new TestY;

?>
===DONE===
--EXPECT--
closure called with class TestX
Autoloader('d1') called with TestX
WorkingAutoloader() called with TestX
WorkingAutoloader() called with TestY
===DONE===