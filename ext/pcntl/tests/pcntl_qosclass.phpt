--TEST--
pcntl_getqos_class()/pcntl_setqos_class()
--EXTENSIONS--
pcntl
--SKIPIF--
<?php
if (!function_exists("pcntl_getqos_class")) die("skip pcntl_getqos_class() is not available");
if (getenv('SKIP_REPEAT')) die("skip Not repeatable");
?>
--FILE--
<?php
pcntl_setqos_class(Pcntl\QosClass::Default);
var_dump(Pcntl\QosClass::Default === pcntl_getqos_class());
pcntl_setqos_class(Pcntl\QosClass::Background);
var_dump(Pcntl\QosClass::Background == pcntl_getqos_class());
?>
--EXPECT--
bool(true)
bool(true)
