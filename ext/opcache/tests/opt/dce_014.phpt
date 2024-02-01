--TEST--
DCE may remove FREE after FETCH_LIST_W
--FILE--
<?php
function () {
    $a = $r[][] = $r = [] & $y;
    +list(&$y) = $a;
}
?>
DONE
--EXPECT--
DONE
