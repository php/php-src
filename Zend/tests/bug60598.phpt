--TEST--
Bug #60598: cli/apache sapi segfault on objects manipulation
--FILE--
<?
define('OBJECT_COUNT', 130000);

class Object {
    private static $world = array();
    private static $maxGuid = 0;
    protected $_guid = null;
    public function __construct() {
         self::$world[$this->_guid = self::$maxGuid++] = $this;
    }
    public function __destruct() {
         unset(self::$world[$this->_guid]);
    }
}

for ($i = 0; $i < OBJECT_COUNT; ++$i) {
    new Object();
}

echo 'Working';

--EXPECTF--
Working
