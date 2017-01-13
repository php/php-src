--TEST--
Bug #71604: Aborted Generators continue after nested finally (2)
--FILE--
<?php

function gen() {
    try {
        throw new Exception(1);
    } finally {
        try {
            throw new Exception(2);
        } finally {
            try {
                yield;
            } finally {
            }
        }
    }
}

try {
    gen()->rewind();
} catch (Exception $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
Exception: 1 in %s:%d
Stack trace:
#0 [internal function]: gen()
#1 %s(%d): Generator->rewind()
#2 {main}

Next Exception: 2 in %s:%d
Stack trace:
#0 [internal function]: gen()
#1 %s(%d): Generator->rewind()
#2 {main}
