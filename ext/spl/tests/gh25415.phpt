--TEST--
GH-25415 (Segfault in FilterIterator::next() after the subclass catches an exception from parent::__construct())
--CREDITS--
marinelay
--FILE--
<?php

function g() { yield 1; }

$gen = g();
foreach ($gen as $v) {}

$iterators = [
    new class($gen) extends FilterIterator {
        public function __construct(Iterator $it) {
            try {
                parent::__construct($it);
            } catch (Exception $e) {
                echo $e->getMessage(), "\n";
            }
        }
        public function accept(): bool { return true; }
    },
    new class($gen) extends IteratorIterator {
        public function __construct(Iterator $it) {
            try {
                parent::__construct($it);
            } catch (Exception $e) {
                echo $e->getMessage(), "\n";
            }
        }
    },
    new class($gen) extends NoRewindIterator {
        public function __construct(Iterator $it) {
            try {
                parent::__construct($it);
            } catch (Exception $e) {
                echo $e->getMessage(), "\n";
            }
        }
    },
];

foreach ($iterators as $it) {
    var_dump($it->valid(), $it->key(), $it->current());
    try {
        $it->next();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECT--
Cannot traverse an already closed generator
Cannot traverse an already closed generator
Cannot traverse an already closed generator
bool(false)
NULL
NULL
The inner constructor wasn't initialized with an iterator instance
bool(false)
NULL
NULL
The inner constructor wasn't initialized with an iterator instance
bool(false)
NULL
NULL
The inner constructor wasn't initialized with an iterator instance
