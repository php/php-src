--TEST--
SplHeap::next() write lock
--CREDITS--
cnitlrt
--FILE--
<?php

class EvilPQ extends SplPriorityQueue {
    private bool $did = false;

    public function compare(mixed $p1, mixed $p2): int {
        if (!$this->did) {
            $this->did = true;
            // Re-entrant write during internal heap insertion comparison.
            if (!$this->isEmpty()) {
                $this->next(); // no write-lock validation
            }
        }
        return parent::compare($p1, $p2);
    }
}

$q = new EvilPQ();
try {
    for ($i = 0; $i < 200; $i++) {
        $q->insert("d$i", 100 - $i);
    }
} catch (RuntimeException $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
RuntimeException: Heap cannot be changed when it is already being modified.
