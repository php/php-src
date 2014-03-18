--TEST--
Test HashContext
--SKIPIF--
<?php extension_loaded('hash') or die('skip: hash extension not loaded.'); ?>
--FILE--
<?php

$context = new HashContext("md5");
$context->update("data");

$copy_context = clone $context;

echo $context->final(), "\n";

$copy_context->update("data");
echo $copy_context->final(), "\n";

?>
===Done===
--EXPECTF--
8d777f385d3dfec8815d20f7496026dc
511ae0b1c13f95e5f08f1a0dd3da3d93
===Done===