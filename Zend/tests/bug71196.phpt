--TEST--
Bug #71196 (Memory leak with out-of-order live ranges)
--FILE--
<?php
try  {
        $a = "1";
        [1, (y().$a.$a) . ($a.$a)];
} catch (Error $e) {
        var_dump($e->getMessage());
}
?>
--EXPECT--
string(30) "Call to undefined function y()"
