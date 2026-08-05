--TEST--
Asymmetric set visibility survives optimizer cache-slot sharing between $this reads and writes
--FILE--
<?php
class P {
    public private(set) string $prop = 'default';
}

class C extends P {
    public function test() {
        // The read populates a runtime cache slot for $this->prop; the write
        // below must not reuse that (read-kind) resolution to bypass the
        // set-visibility check when the optimizer shares property slots.
        var_dump($this->prop);
        try {
            $this->prop = 'overwritten';
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        var_dump($this->prop);
    }
}

$c = new C;
$c->test();
$c->test();
?>
--EXPECT--
string(7) "default"
Cannot modify private(set) property P::$prop from scope C
string(7) "default"
string(7) "default"
Cannot modify private(set) property P::$prop from scope C
string(7) "default"
