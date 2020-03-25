--TEST--
Bug #22510 (segfault among complex references)
--FILE--
<?php
class foo
{
    public $list = array();

    function finalize() {
        print __CLASS__."::".__FUNCTION__."\n";
        $cl = &$this->list;
    }

    function &method1() {
        print __CLASS__."::".__FUNCTION__."\n";
        return @$this->foo;
    }

    function &method2() {
        print __CLASS__."::".__FUNCTION__."\n";
        return $this->foo;
    }

    function method3() {
        print __CLASS__."::".__FUNCTION__."\n";
        return @$this->foo;
    }
}

class bar
{
    function run1() {
        print __CLASS__."::".__FUNCTION__."\n";
        $this->instance = new foo();
        $this->instance->method1($this);
        $this->instance->method1($this);
    }

    function run2() {
        print __CLASS__."::".__FUNCTION__."\n";
        $this->instance = new foo();
        $this->instance->method2($this);
        $this->instance->method2($this);
    }

    function run3() {
        print __CLASS__."::".__FUNCTION__."\n";
        $this->instance = new foo();
        $this->instance->method3($this);
        $this->instance->method3($this);
    }
}

function ouch(&$bar) {
    print __FUNCTION__."\n";
    @$a = $a;
    $bar->run1();
}

function ok1(&$bar) {
    print __FUNCTION__."\n";
    $bar->run1();
}

function ok2(&$bar) {
    print __FUNCTION__."\n";
    @$a = $a;
    $bar->run2();
}

function ok3(&$bar) {
    print __FUNCTION__."\n";
    @$a = $a;
    $bar->run3();
}

$foo = new bar();
$bar =& $foo;
ok1($bar);
$bar->instance->finalize();
print "done!\n";
ok2($bar);
$bar->instance->finalize();
print "done!\n";
ok3($bar);
$bar->instance->finalize();
print "done!\n";
ouch($bar);
$bar->instance->finalize();
print "I'm alive!\n";
?>
--EXPECTF--
ok1
bar::run1
foo::method1

Notice: Only variable references should be returned by reference in %s on line %d
foo::method1

Notice: Only variable references should be returned by reference in %s on line %d
foo::finalize
done!
ok2
bar::run2
foo::method2
foo::method2
foo::finalize
done!
ok3
bar::run3
foo::method3
foo::method3
foo::finalize
done!
ouch
bar::run1
foo::method1

Notice: Only variable references should be returned by reference in %s on line %d
foo::method1

Notice: Only variable references should be returned by reference in %s on line %d
foo::finalize
I'm alive!
