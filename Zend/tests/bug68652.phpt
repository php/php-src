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
        if (!isset(self::$instance)) return;
        Foo::getInstance();
    }
}


$foo = new Foo();
?>
OK
--EXPECT--
OK
