--TEST--
Self-referencing map entry GC - 007
--FILE--
<?php

class Canary extends stdClass
{
    public function __construct(public string $name)
    {
    }

    function __destruct()
    {
        echo $this->name."\n";
    }
}

$container = new Canary('container');
$canary = new Canary('canary');
$container->canary = $canary;

$map = new \WeakMap();
$map[$canary] = $container;

echo 1;
unset($container, $canary);
gc_collect_cycles();
echo 2;

?>
--EXPECT--
1container
canary
2
