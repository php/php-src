--TEST--
Standard behaviour of __DIR__
--FILE--
<?php
echo __DIR__ . "\n";
echo __DIR__ . "\n";
?>
--EXPECTF--
%stests%sconstants
%stests%sconstants
