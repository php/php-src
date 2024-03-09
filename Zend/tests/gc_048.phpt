--TEST--
GC 048: Test GC is rerun when one or more destructor is called
--FILE--
<?php

class CycleWithoutDestructor
{
    private \stdClass $cycleRef;

    public function __construct()
    {
        $this->cycleRef = new \stdClass();
        $this->cycleRef->x = $this;
    }
}

class CycleWithDestructor extends CycleWithoutDestructor
{
    public function __construct()
    {
        parent::__construct();
    }

    public function __destruct()
    {
        new CycleWithoutDestructor();
    }
}

var_dump(gc_status()['runs']);

echo "---\n";

$cycleWithoutDestructor = new CycleWithoutDestructor();
$cycleWithDestructor = new CycleWithDestructor();
var_dump(gc_status()['runs']);
gc_collect_cycles();
var_dump(gc_status()['runs']);

echo "---\n";

unset($cycleWithoutDestructor);
var_dump(gc_status()['runs']);
gc_collect_cycles();
var_dump(gc_status()['runs']);

echo "---\n";

unset($cycleWithDestructor);
var_dump(gc_status()['runs']);
gc_collect_cycles();
var_dump(gc_status()['runs']);
--EXPECT--
int(0)
---
int(0)
int(1)
---
int(1)
int(2)
---
int(2)
int(4)
