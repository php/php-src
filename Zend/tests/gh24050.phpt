--TEST--
GH-24050 (GC frees an object still held by a resurrected closure when (object) and use share an array)
--CREDITS--
OllieCrook
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
    public $castObject;
    public $resurrector;
    public $self;
}

$array = ['victim' => new stdClass(), 'key' => 'value'];

$holder = new Holder();
$resurrector = new Resurrector();
$holder->castObject = (object) $array;
$resurrector->closure = function () use ($array) {
    return $array;
};
$holder->resurrector = $resurrector;
$holder->self = $holder;
$resurrector->self = $resurrector;

unset($array);
gc_collect_cycles();

unset($holder, $resurrector);
gc_collect_cycles();
gc_collect_cycles();

var_dump($resurrected()['victim']);
?>
--EXPECT--
object(stdClass)#1 (0) {
}
