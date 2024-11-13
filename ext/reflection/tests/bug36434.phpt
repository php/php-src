--TEST--
Reflection Bug #36434 (Properties from parent class fail to identify their true origin)
--FILE--
<?php
class ancestor
{
    public $ancestor = 0;
    function __construct()
    {
        return $this->ancestor;
    }
}
class foo extends ancestor
{
    public $bar = "1";
    function __construct()
    {
        return $this->bar;
    }
}

$r = new ReflectionClass('foo');
foreach ($r->GetProperties() as $p)
{
    echo $p->getName(). " ". $p->getDeclaringClass()->getName()."\n";
}

?>
--EXPECT--
bar foo
ancestor ancestor
