--TEST--
GH-13670 003
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
}

class Canary {
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        global $shutdown;
        if (!$shutdown) {
            work();
        }
    }
}

function work() {
    global $objs, $defaultThreshold;
    new Canary();
    // Create some collectable garbage so the next run will not adjust
    // threshold
    for ($i = 0; $i < 100; $i++) {
        new Cycle();
    }
    // Add potential garbage to buffer
    foreach (array_slice($objs, 0, $defaultThreshold) as $obj) {
        $o = $obj;
    }
}

$defaultThreshold = gc_status()['threshold'];
$objs = [];
for ($i = 0; $i < $defaultThreshold*2; $i++) {
    $obj = new stdClass;
    $objs[] = $obj;
}

work();

// Result of array_slice() is a tmpvar that will be checked by
// zend_gc_check_root_tmpvars()
foreach (array_slice($objs, -10) as $obj) {
    $o = $obj;
}

$st = gc_status();

if ($st['runs'] > 10) {
    var_dump($st);
}
?>
==DONE==
--EXPECT--
==DONE==
