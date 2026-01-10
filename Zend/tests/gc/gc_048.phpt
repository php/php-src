--TEST--
GC 048: Objects with destructor are collected without delay
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

echo "---\n";

$cycleWithoutDestructor = new CycleWithoutDestructor();
$cycleWithoutDestructorWeak = \WeakReference::create($cycleWithoutDestructor);
$cycleWithDestructor = new CycleWithDestructor();
$cycleWithDestructorWeak = \WeakReference::create($cycleWithDestructor);
gc_collect_cycles();

echo "---\n";

unset($cycleWithoutDestructor);
var_dump($cycleWithoutDestructorWeak->get() !== null);
gc_collect_cycles();
var_dump($cycleWithoutDestructorWeak->get() !== null);

echo "---\n";

unset($cycleWithDestructor);
var_dump($cycleWithDestructorWeak->get() !== null);
gc_collect_cycles();
var_dump($cycleWithDestructorWeak->get() !== null);
--EXPECT--
---
---
bool(true)
bool(false)
---
bool(true)
bool(false)
