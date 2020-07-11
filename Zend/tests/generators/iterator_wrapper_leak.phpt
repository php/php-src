--TEST--
A generator iterator wrapper involved in a cycle should not leak
--FILE--
<?php

class Test {
    public function method() {
        $this->gen1 = (function () {
            yield 1;
            yield 2;
            yield 3;
        })();
        $gen2 = function() {
            foreach ($this->gen1 as $x) {
                echo "$x\n";
                yield $x;
            }
        };
        $this->gen2 = $gen2();
        foreach ($this->gen2 as $x) {
            if ($x == 2) {
                break;
            }
        }
    }
}
(new Test)->method();
gc_collect_cycles();

?>
--EXPECT--
1
2
