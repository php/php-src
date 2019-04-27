--TEST--
bc_raisemod's mod can't be zero and expo can't be negative
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--SKIPIF--
<?php if(!extension_loaded('bcmath')) die('skip bcmath extension not loaded'); ?>
--FILE--
<?php var_dump(bcpowmod('1', '-1', '0')); ?>
--EXPECT--
bool(false)
