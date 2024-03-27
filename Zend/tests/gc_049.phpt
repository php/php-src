--TEST--
GC 049: Test objects created during GC cycle does not participate in the same cycle
--FILE--
<?php

class CycleWithDestructor
{
    private \Closure $destructorFx;

    private \stdClass $cycleRef;

    public function __construct(\Closure $destructorFx)
    {
        $this->destructorFx = $destructorFx;
        $this->cycleRef = new \stdClass();
        $this->cycleRef->x = $this;
    }

    public function __destruct()
    {
        ($this->destructorFx)();
    }
}

$isSecondGcRerun = false; // https://github.com/php/php-src/commit/b58d74547f
$createFx = static function () use (&$createFx, &$isSecondGcRerun): void {
    $destructorFx = static function () use (&$createFx, &$isSecondGcRerun): void {
        if (!gc_status()['running']) {
            echo "gc shutdown\n";
            return;
        }

        echo "gc" . ($isSecondGcRerun ? ' rerun' : '') . "\n";

        $isSecondGcRerun = !$isSecondGcRerun;

        $createFx();
    };

    new CycleWithDestructor($destructorFx);
};

$createFx();
gc_collect_cycles();
gc_collect_cycles();
gc_collect_cycles();
echo "---\n";
gc_collect_cycles();
gc_collect_cycles();
gc_collect_cycles();
echo "---\n";
?>
--EXPECT--
gc
gc rerun
gc
gc rerun
gc
gc rerun
---
gc
gc rerun
gc
gc rerun
gc
gc rerun
---
gc shutdown
