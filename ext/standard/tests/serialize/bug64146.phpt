--TEST--
Bug #64146 (serialize incorrectly saving objects when they are cloned)
--FILE--
<?php

echo "Test\n";

class A
{
    public $a = array();

    public function __construct()
    {
        $this->a[] = new B(1);
        $this->a[] = new B(2);
    }
}

class B implements Serializable
{
    public $b;

    public function __construct($c)
    {
        $this->b = new C($c);
    }

    public function serialize()
    {
        return serialize(clone $this->b);
    }

    public function unserialize($data)
    {
        $this->b = unserialize($data);
    }
}

class C
{
    public $c;

    public function __construct($c)
    {
        $this->c = $c;
    }
}

$a = unserialize(serialize(new A()));

print $a->a[0]->b->c . "\n";
print $a->a[1]->b->c . "\n";

?>
Done
--EXPECT--
Test
1
2
Done
