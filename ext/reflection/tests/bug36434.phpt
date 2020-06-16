--TEST--
Reflection Bug #36434 (Properties from parent class fail to indetify their true origin)
--FILE--
<?php
class ancester
{
    public $ancester = 0;
}
class foo extends ancester
{
    public $bar = "1";
}

$r = new ReflectionClass('foo');
foreach ($r->getProperties() as $p)
{
    echo $p->getName(). " ". $p->getDeclaringClass()->getName()."\n";
}

?>
--EXPECT--
bar foo
ancester ancester
