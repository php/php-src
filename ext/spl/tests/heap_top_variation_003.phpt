--TEST--
SPL: SplHeap top of empty heap
--CREDITS--
Mark Schaschke (mark@fractalturtle.com)
TestFest London May 2009
--FILE--
<?php
$h = new SplMinHeap();
try {
    $h->top();
} catch (Exception $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Can't peek at an empty heap
