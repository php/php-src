--TEST--
Bug #69446 (GC leak relating to removal of nested data after dtors run)
--INI--
zend.enable_gc = 1
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
--EXPECT--
object(bad)#4 (1) {
  ["_private"]=>
  array(1) {
    [0]=>
    string(3) "php"
  }
}
