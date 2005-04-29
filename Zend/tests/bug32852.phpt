--TEST--
Bug #32852 (Crash with singleton and __destruct when zend.ze1_compatibility_mode = On)
--INI--
zend.ze1_compatibility_mode=on
error_reporting=4095
--FILE--
<?php
class crashme {
    private static $instance = null;

    public function __construct() {
        self::$instance = $this;
    }

    public function __destruct() {
        echo "i'm called\n";
    }

    public static function singleton() {
        if (!isset(self::$instance)) {
            self::$instance = new crashme();
        }
        return self::$instance;
    }
}

crashme::singleton();
?>
--EXPECTF--
Strict Standards: Implicit cloning object of class 'crashme' because of 'zend.ze1_compatibility_mode' in %sbug32852.php on line 6
i'm called

Strict Standards: Implicit cloning object of class 'crashme' because of 'zend.ze1_compatibility_mode' in %sbug32852.php on line 15
i'm called

Strict Standards: Implicit cloning object of class 'crashme' because of 'zend.ze1_compatibility_mode' in %sbug32852.php on line 17
i'm called
i'm called
