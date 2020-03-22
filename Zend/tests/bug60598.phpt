--TEST--
Bug #60598 (cli/apache sapi segfault on objects manipulation)
--FILE--
<?php
define('OBJECT_COUNT', 10000);

$containers = array();

class ObjectOne {
    protected $_guid = 0;
    public function __construct() {
        global $containers;
        $this->guid = 1;
        $containers[spl_object_hash($this)] = $this;
    }
    public function __destruct() {
        global $containers;
        $containers[spl_object_hash($this)] = NULL;
    }
}

for ($i = 0; $i < OBJECT_COUNT; ++$i) {
    new ObjectOne();
}

// You probably won't see this because of the "zend_mm_heap corrupted"
?>
If you see this, try to increase OBJECT_COUNT to 100,000
--EXPECT--
If you see this, try to increase OBJECT_COUNT to 100,000
