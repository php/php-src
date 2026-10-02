--TEST--
GH-24050 (GC frees an object still held by a resurrected closure when (array) and use share a properties table)
--FILE--
<?php

class Resurrector
{
    public $closure;
    public $self;

    public function __destruct()
    {
        $GLOBALS['resurrected'] = $this->closure;
    }
}

class Holder
{
    public $obj;
    public $resurrector;
    public $self;
}

$obj = new stdClass();
$obj->victim = new stdClass();

$holder = new Holder();
$resurrector = new Resurrector();
$holder->obj = $obj;
$array = (array) $obj;
$resurrector->closure = function () use ($array) {
    return $array;
};
$holder->resurrector = $resurrector;
$holder->self = $holder;
$resurrector->self = $resurrector;

unset($array, $obj);
gc_collect_cycles();

unset($holder, $resurrector);
gc_collect_cycles();
gc_collect_cycles();

var_dump($resurrected()['victim']);
?>
--EXPECT--
object(stdClass)#2 (0) {
}
