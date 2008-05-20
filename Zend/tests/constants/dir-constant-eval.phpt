--TEST--
__DIR__ constant used with eval()
--FILE--
<?php
eval('echo __DIR__ . "\n";');
?>
--EXPECTF--
%stests%sconstants
