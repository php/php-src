--TEST--
Registering two different instance of a class as an autoloader should work
--FILE--
<?php
class MyAutoloader {
    function __construct(private string $directory_to_use) {}
    function autoload($class_name) {
        echo $this->directory_to_use, "\n";
    }
}

$autoloader1 = new MyAutoloader('dir1');
autoload_register_function(array($autoloader1, 'autoload'));

$autoloader2 = new MyAutoloader('dir2');
autoload_register_function(array($autoloader2, 'autoload'));

var_dump(autoload_list_function());
var_dump(function_exists('NonExisting'));

?>
--EXPECT--
array(2) {
  [0]=>
  array(2) {
    [0]=>
    object(MyAutoloader)#1 (1) {
      ["directory_to_use":"MyAutoloader":private]=>
      string(4) "dir1"
    }
    [1]=>
    string(8) "autoload"
  }
  [1]=>
  array(2) {
    [0]=>
    object(MyAutoloader)#2 (1) {
      ["directory_to_use":"MyAutoloader":private]=>
      string(4) "dir2"
    }
    [1]=>
    string(8) "autoload"
  }
}
dir1
dir2
bool(false)
