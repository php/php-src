--TEST--
Bug #70228 (memleak if return in finally block)
--FILE--
<?php
function test() {
    try {
        throw new Exception(1);
    } finally {
        try {
            throw new Exception(2);
        } finally {
            return 42;
        }
    }
}

var_dump(test());
?>
--EXPECT--
int(42)
