--TEST--
GH-16408: Warning emitted during array to string conversion in optimizer
--FILE--
<?php
$counter = 0;
ob_start(function ($buffer) use (&$c, &$counter) {
        $c = 0;
        ++$counter;
}, 1);
$c .= [];
$c .= [];
ob_end_clean();
echo $counter . "\n";
?>
--EXPECT--
3
