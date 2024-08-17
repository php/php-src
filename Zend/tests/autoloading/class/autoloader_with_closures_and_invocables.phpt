--TEST--
Autoloading with closures and invocables
--FILE--
<?php
$closure = function ($name) {
    echo 'autoload(' . $name . ")\n";
};

class Autoloader {
  public function __construct(private string $dir) {}
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

autoload_register_class($closure);
autoload_register_class($al1);
autoload_register_class($al2);

var_dump(autoload_list_class());

$x = new TestX;

autoload_unregister_class($closure);
autoload_unregister_class($al1);

$y = new TestY;

?>
--EXPECTF--
array(3) {
  [0]=>
  object(Closure)#1 (4) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["parameter"]=>
    array(1) {
      ["$name"]=>
      string(10) "<required>"
    }
  }
  [1]=>
  object(Autoloader)#2 (1) {
    ["dir":"Autoloader":private]=>
    string(2) "d1"
  }
  [2]=>
  object(WorkingAutoloader)#3 (0) {
  }
}
autoload(TestX)
Autoloader('d1') called with TestX
WorkingAutoloader() called with TestX
WorkingAutoloader() called with TestY
