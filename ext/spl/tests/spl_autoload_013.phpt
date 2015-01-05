--TEST--
SPL: spl_autoload_functions() with closures and invokables
--FILE--
<?php
$closure = function($class) {
  echo "a called\n";
};

class Autoloader {
  private $dir;
  public function __construct($dir) {
    $this->dir = $dir;
  }
  public function __invoke($class) {
    var_dump("{$this->dir}/$class.php");
  }
}

$al1 = new Autoloader('d1');
$al2 = new Autoloader('d2');

spl_autoload_register($closure);
spl_autoload_register($al1);
spl_autoload_register($al2);

var_dump(spl_autoload_functions());

?>
===DONE===
--EXPECTF--
array(3) {
  [0]=>
  object(Closure)#%d (1) {
    ["parameter"]=>
    array(1) {
      ["$class"]=>
      string(10) "<required>"
    }
  }
  [1]=>
  object(Autoloader)#%d (1) {
    ["dir":"Autoloader":private]=>
    string(2) "d1"
  }
  [2]=>
  object(Autoloader)#%d (1) {
    ["dir":"Autoloader":private]=>
    string(2) "d2"
  }
}
===DONE===