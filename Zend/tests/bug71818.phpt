--TEST--
Bug #71818 (Memory leak when array altered in destructor)
--INI--
zend.enable_gc = 1
--FILE--
<?php
class MemoryLeak
{
    public function __construct()
    {
        $this->things[] = $this;
    }

    public function __destruct()
    {
        $this->things[] = null;
    }

    private $things = [];
}

ini_set('memory_limit', '10M');

for ($i = 0; $i < 100000; ++$i) {
    $obj = new MemoryLeak();
}
echo "OK\n";
?>
--EXPECT--
OK
