--TEST--
Bug #53748 (Using traits lead to a segmentation fault)
--FILE--
<?php

trait Singleton {
  protected static $instances=array();
  abstract protected function __construct($config);
  public static function getInstance($config) {
    if (!isset(self::$instances[$serialize = serialize($config)])) {
      self::$instances[$serialize] = new self($config);
    }
    return self::$instances[$serialize];
  }
}

class MyHelloWorld {
  use Singleton;
  public function __construct($config)
  {
    var_dump( $config);
  }
}


$o= myHelloWorld::getInstance(1);
$o= myHelloWorld::getInstance(1);
$o= myHelloWorld::getInstance(2);
$o= myHelloWorld::getInstance(array(1=>2));
$o= myHelloWorld::getInstance(array(1=>2));

?>
--EXPECTF--
int(1)
int(2)
array(1) {
  [1]=>
  int(2)
}
