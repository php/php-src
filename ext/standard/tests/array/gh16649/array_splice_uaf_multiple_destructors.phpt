--TEST--
GH-16649: array_splice UAF with multiple destructors
--FILE--
<?php
class MultiDestructor {
    public $id;

    function __construct($id) {
        $this->id = $id;
    }

    function __destruct() {
        global $arr;
        echo "Destructor {$this->id} called\n";
        if ($this->id == 2) {
            $arr = null;
        }
    }
}

$arr = ["start", new MultiDestructor(1), new MultiDestructor(2), "end"];

try {
    array_splice($arr, 1, 2);
    echo "ERROR: Should have thrown exception\n";
} catch (Error $e) {
    echo "Exception caught: " . $e->getMessage() . "\n";
}
?>
--EXPECT--
Destructor 1 called
Destructor 2 called
Exception caught: Array was modified during array_splice operation
