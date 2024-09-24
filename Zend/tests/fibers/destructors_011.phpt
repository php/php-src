--TEST--
Fibers in destructors 011: gc collection after the dtor fiber is dtor
--FILE--
<?php

class SimpleCycle {
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        printf("%s\n", __METHOD__);
    }
}

class CreateGarbageInDtor {
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        printf("%s\n", __METHOD__);
        // Create an object whose dtor will be called after the dtor fiber's
        new CollectCyclesInFiberInDtor();
    }
}

class CollectCyclesInFiberInDtor {
    public $self;
    public function __construct() {
        $this->self = $this;
    }
    public function __destruct() {
        printf("%s\n", __METHOD__);
        new SimpleCycle();
        print "Collecting cycles\n";
        $f = new Fiber(function () {
            gc_collect_cycles();
        });
        $f->start();
        print "Done collecting cycles\n";
    }
}

register_shutdown_function(function () {
    print "Shutdown\n";
});

// Create a cycle
new SimpleCycle();

// Collect cycles to create the dtor fiber
$f = new Fiber(function () {
    gc_collect_cycles();
});
$f->start();

// Create an object whose dtor will be called during shutdown
// (by zend_objects_store_call_destructors)
new CreateGarbageInDtor();

?>
--EXPECT--
SimpleCycle::__destruct
Shutdown
CreateGarbageInDtor::__destruct
CollectCyclesInFiberInDtor::__destruct
Collecting cycles
SimpleCycle::__destruct
Done collecting cycles
