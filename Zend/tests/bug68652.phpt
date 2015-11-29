--TEST--
Bug #68652 (segmentation fault in destructor)
--FILE--
<?php
class Foo {

    private static $instance;
    public static function getInstance() {
        if (isset(self::$instance)) {
            return self::$instance;
        }
        return self::$instance = new self();
    }

    public function __destruct() {
        Bar::getInstance();
    }
}

class Bar {

    private static $instance;
    public static function getInstance() {
        if (isset(self::$instance)) {
            return self::$instance;
        }
        return self::$instance = new self();
    }

    public function __destruct() {
        Foo::getInstance();
    }
}


$foo = new Foo();
?>
--EXPECTF--
Fatal error: Uncaught Error: Access to undeclared static property: Bar::$instance in %sbug68652.php:%d
Stack trace:
#0 %s(%d): Bar::getInstance()
#1 [internal function]: Foo->__destruct()
#2 {main}
  thrown in %sbug68652.php on line %d

