--TEST--
ZE2 factory and singleton, test 2
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php
class test {
  protected $x;

  static private $test = NULL;
  static private $cnt = 0;

  static function factory($x) {
    if (test::$test) {
      return test::$test;
    } else {
      test::$test = new test($x);
      return test::$test;
    }
  }

  protected function __construct($x) {
    test::$cnt++;
    $this->x = $x;
  }

  static function destroy() {
    test::$test = NULL;
  }

  protected function __destruct() {
  	test::$cnt--;
  }

  public function get() {
    return $this->x;
  }

  static public function getX() {
    if (test::$test) {
      return test::$test->x;
    } else {
      return NULL;
    }
  }
  
  static public function count() {
    return test::$cnt;
  }
}

echo "Access static members\n";
var_dump(test::getX());
var_dump(test::count());

echo "Create x and y\n";
$x = test::factory(1);
$y = test::factory(2);
var_dump(test::getX());
var_dump(test::count());
var_dump($x->get());
var_dump($y->get());

echo "Destruct x\n";
$x = NULL;
var_dump(test::getX());
var_dump(test::count());
var_dump($y->get());

echo "Destruct y\n";
$y = NULL;
var_dump(test::getX());
var_dump(test::count());

//echo "Destruct static\n";
//test::destroy();
//var_dump(test::getX());
//var_dump(test::count());

echo "Done\n";
?>
--EXPECT--
Access static members
NULL
int(0)
Create x and y
int(1)
int(1)
int(1)
int(1)
Destruct x
int(1)
int(1)
int(1)
Destruct y
int(1)
int(1)
Done

Warning: Call to protected test::__destruct() from context '' during shutdown ignored in Unknown on line 0
