--TEST--
Bug #29689 (default value of protected member overrides default value of private)
--FILE--
<?php
class foo {
    private $foo = 'foo';

    function printFoo()
    {
        echo __CLASS__, ': ', $this->foo, "\n";
    }
}

class bar extends foo {
    protected $foo = 'bar';

    function printFoo()
    {
        parent::printFoo();
        echo __CLASS__, ': ', $this->foo, "\n";
    }
}

class baz extends bar {
    protected $foo = 'baz';
}

class bar2 extends foo {
    function printFoo()
    {
        parent::printFoo();
        echo __CLASS__, ': ', $this->foo, "\n";
    }
}

class baz2 extends bar2 {
    protected $foo = 'baz2';
}

$bar = new bar;
$bar->printFoo();
echo "---\n";
$baz = new baz();
$baz->printFoo();
echo "---\n";
$baz = new baz2();
$baz->printFoo();
?>
--EXPECT--
foo: foo
bar: bar
---
foo: foo
bar: baz
---
foo: foo
bar2: baz2
