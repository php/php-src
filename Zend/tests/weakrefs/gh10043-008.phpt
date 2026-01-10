--TEST--
Self-referencing map entry GC - 008
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

$canary = new Canary('canary');

$map = new \WeakMap();
$map[$canary] = $canary;

echo 1;
unset($canary);
gc_collect_cycles();
echo 2;

?>
--EXPECT--
1canary
2
