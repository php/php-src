--TEST--
Bug #34045 (Buffer overflow with serialized object)
--FILE--
<?php
class BasicSingleton
{
    private static $instance;

    public function __wakeup() {
        self::$instance = $this;
    }

    public static function singleton() {
        if (!(self::$instance instanceof BasicSingleton)) {
            $c = __CLASS__;
            self::$instance = new $c;
        }
        return self::$instance;
    }
}

$db = BasicSingleton::singleton();
$db_str = serialize($db);
$db2 = unserialize($db_str);
echo "ok\n";
?>
--EXPECT--
ok
