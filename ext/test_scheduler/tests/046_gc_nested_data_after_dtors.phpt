--TEST--
Bug #69446 (GC leak relating to removal of nested data after dtors run) — under test_scheduler
--INI--
test_scheduler.enable=1
zend.enable_gc = 1
--EXTENSIONS--
test_scheduler
--FILE--
<?php
$bar = NULL;
class bad
{
    public $_private = array();

    public function __construct()
    {
        $this->_private[] = 'php';
    }

    public function __destruct()
    {
        global $bar;
        $bar = $this;
    }
}

$foo = new stdclass;
$foo->foo = $foo;
$foo->bad = new bad;

unserialize(serialize($foo));
//unset($foo);

gc_collect_cycles();
var_dump($bar);
?>
--EXPECTF--
object(bad)#%d (1) {
  ["_private"]=>
  array(1) {
    [0]=>
    string(3) "php"
  }
}
