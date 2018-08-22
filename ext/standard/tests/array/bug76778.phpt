--TEST--
Bug #76778 (array_reduce leaks memory if callback throws exception)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--INI--
memory_limit=32M
--FILE--
<?php
for ($i = 0; $i < 100; $i++) {
    try {
        array_reduce(
            [1],
            function ($carry, $item) {
                throw new Exception;
            },
            range(1, 200000)
        );
    } catch (Exception $e) {
    }
}
?>
===DONE===
--EXPECT--
===DONE===
