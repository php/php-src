--TEST--
Bug #69172 array_slice behaviour BC in PHP7
--FILE--
<?php 
$stack[] = 'wew';
$stack[] = 'wew';
$stack[] = 'wew';
$stack[] = 'wew';
$stack = array_slice($stack, 0, - 2);
$stack[] = 'wew';

print_r($stack);
?>
--EXPECT--
Array
(
    [0] => wew
    [1] => wew
    [2] => wew
)
