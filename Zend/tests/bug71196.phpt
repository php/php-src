--TEST--
Bug #71196 (Memory leak with out-of-order live ranges)
--FILE--
<?php
try  {
        $a = "1";
        [1, (y().$a.$a) . ($a.$a)];
} catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Call to undefined function y()
