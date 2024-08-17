--TEST--
Autoloading with closures and invocables
--FILE--
<?php
$closure = function ($name) {
    echo 'autoload(' . $name . ")\n";
};

class Autoloader {
  public function __construct(private string $dir) {}
  public function __invoke($func) {
    echo ("Autoloader('{$this->dir}') called with $func\n");
  }
}

class WorkingAutoloader {
  public function __invoke($func) {
    echo ("WorkingAutoloader() called with $func\n");
    eval("function $func() { }");
  }
}

$al1 = new Autoloader('d1');
$al2 = new WorkingAutoloader('d2');

autoload_register_function($closure);
autoload_register_function($al1);
autoload_register_function($al2);

var_dump(autoload_list_function());

$foo = foo();

autoload_unregister_function($closure);
autoload_unregister_function($al1);

$bar = bar();

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
autoload(foo)
Autoloader('d1') called with foo
WorkingAutoloader() called with foo
WorkingAutoloader() called with bar
