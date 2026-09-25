--TEST--
GH-13670 001
--SKIPIF--
<?php
// gc_threshold is global state
if (getenv('SKIP_REPEAT')) die('skip Not repeatable');
?>
--FILE--
<?php

register_shutdown_function(function () {
    global $shutdown;
    $shutdown = true;
});

class Cycle {
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        global $shutdown;
        if (!$shutdown) {
            new Cycle();
        }
    }
}

$defaultThreshold = gc_status()['threshold'];
for ($i = 0; $i < $defaultThreshold+1; $i++) {
    new Cycle();
}

$objs = [];
for ($i = 0; $i < 100; $i++) {
    $obj = new stdClass;
    $objs[] = $obj;
}

$st = gc_status();

if ($st['runs'] > 10) {
    var_dump($st);
}
?>
==DONE==
--EXPECT--
==DONE==
