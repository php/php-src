--TEST--
GH-16408: Array to string conversion warning emitted in optimizer
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
--EXPECTF--
Deprecated: main(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d

Deprecated: main(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d

Deprecated: ob_end_clean(): Returning a non-string result from user output handler {closure:%s:%d} is deprecated in %s on line %d
3
