--TEST--
Bug #27641 (zend.ze1_compatibility_mode = On causes object properties to be misreported)
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 is needed'); ?>
--FILE--
<?php
  class A {
    public $a = "Default for A";
    public $b = "Default for B";

    function __construct($a, $b) {
      $this->a = $a;
      $this->b = $b;
    }
    function A() {
      $args = func_get_args();
      call_user_func_array(Array(&$this, '__construct'), $args);
    }
  }

  $t = new A("New A", "New B");
  print_r($t);
  print_r(get_class_vars(get_class($t)));
  print_r(get_object_vars($t));
?>
--EXPECTF--
Strict Standards: Redefining already defined constructor for class A in %sbug27641.php on line %d
A Object
(
    [a] => New A
    [b] => New B
)
Array
(
    [a] => Default for A
    [b] => Default for B
)
Array
(
    [a] => New A
    [b] => New B
)
