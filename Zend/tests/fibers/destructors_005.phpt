--TEST--
Fibers in destructors 005: Suspended and not resumed destructor
--FILE--
<?php

register_shutdown_function(function () {
    printf("Shutdown\n");
});

class Cycle {
    public $self;
    public function __construct(public int $id) {
        $this->self = $this;
    }
    public function __destruct() {
        printf("%d: Start destruct\n", $this->id);
        try {
            if ($this->id === 0) {
                /* Fiber will be collected by GC because it's not referenced */
                Fiber::suspend(new stdClass);
            } else if ($this->id === 1) {
                /* Fiber will be dtor during shutdown */
                global $f2;
                $f2 = Fiber::getCurrent();
                Fiber::suspend(new stdClass);
            }
        } finally {
            printf("%d: End destruct\n", $this->id);
        }
    }
}

$refs = [];
$f = new Fiber(function () use (&$refs) {
    $refs[] = WeakReference::create(new Cycle(0));
    $refs[] = WeakReference::create(new Cycle(1));
    $refs[] = WeakReference::create(new Cycle(2));
    gc_collect_cycles();
});

$f->start();

gc_collect_cycles();

foreach ($refs as $id => $ref) {
    printf("%d: %s\n", $id, $ref->get() ? 'Live' : 'Collected');
}

?>
--EXPECT--
2: Start destruct
2: End destruct
0: Start destruct
0: End destruct
1: Start destruct
0: Collected
1: Live
2: Collected
Shutdown
1: End destruct
