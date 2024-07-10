--TEST--
nameof operator
--FILE--
<?php
$a = 'test';
echo 'name: ' . nameof($a);
?>
--EXPECT--
name: a