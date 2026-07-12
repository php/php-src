--TEST--
Deferred destructor: exit() in a destructor wins over a throwing destructor in the same flush, either order
--FILE--
<?php
class Exiter {
    function __destruct() {
        echo "Exiter dtor\n";
        exit(7);
    }
}
class Thrower {
    public $id;
    function __construct(int $id) { $this->id = $id; }
    function __destruct() {
        echo "Thrower {$this->id} dtor\n";
        throw new Exception("thrown {$this->id}");
    }
}

register_shutdown_function(function () { echo "shutdown\n"; });

// Thrower deferred before Exiter, then Exiter: exit must win over the pending throw.
$t = new Thrower(1);
$t = null;
$e = new Exiter();
$e = null;

// A safepoint flushes the batch. exit(7) wins; no user code runs afterwards.
(function () {})();

echo "UNREACHABLE\n";
?>
--EXPECT--
Thrower 1 dtor
Exiter dtor
shutdown
