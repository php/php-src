--TEST--
Bug #76778 (array_reduce leaks memory if callback throws exception)
--FILE--
<?php
try {
    array_reduce(
        [1],
        function ($carry, $item) {
            throw new Exception;
        },
        range(1, 3)
    );
} catch (Exception $e) {
}
?>
===DONE===
--EXPECT--
===DONE===
