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
#0 %s(%d): dummy->bar()
#1 %s(%d): dummy->foo1()
#2 %s(%d): dummy->__construct()
