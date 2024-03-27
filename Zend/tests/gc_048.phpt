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
$cycleWithoutDestructorWeak = \WeakReference::create($cycleWithoutDestructor);
$cycleWithDestructor = new CycleWithDestructor();
$cycleWithDestructorWeak = \WeakReference::create($cycleWithDestructor);
var_dump(gc_status()['runs']);
gc_collect_cycles();
var_dump(gc_status()['runs']);

echo "---\n";

unset($cycleWithoutDestructor);
var_dump(gc_status()['runs']);
var_dump($cycleWithoutDestructorWeak->get() !== null);
gc_collect_cycles();
var_dump(gc_status()['runs']);
var_dump($cycleWithoutDestructorWeak->get() !== null);

echo "---\n";

unset($cycleWithDestructor);
var_dump(gc_status()['runs']);
var_dump($cycleWithDestructorWeak->get() !== null);
gc_collect_cycles();
var_dump(gc_status()['runs']);
var_dump($cycleWithDestructorWeak->get() !== null);
--EXPECT--
int(0)
---
int(0)
int(1)
---
int(1)
bool(true)
int(2)
bool(false)
---
int(2)
bool(true)
int(4)
bool(false)
