--TEST--
Bug #70156 (Segfault in zend_find_alias_name)
--FILE--
<?php
trait T1 {
    protected function foo1()
    {
        $this->bar();
    }
}

trait T2 {
    protected function foo2()
    {
        debug_print_backtrace();
    }
}

class dummy {
    use T1 {
        foo1 as private;
    }
    use T2 {
        foo2 as bar;
    }
    public function __construct()
    {
        $this->foo1();
    }
}

new dummy();
?>
--EXPECTF--
#0  dummy->bar() called at [%sbug70156.php:%d]
#1  dummy->foo1() called at [%sbug70156.php:%d]
#2  dummy->__construct() called at [%sbug70156.php:%d]
