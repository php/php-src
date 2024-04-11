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
pcntl_setqos_class(QosClass::Default);
var_dump(QosClass::Default === pcntl_getqos_class());

try {
	pcntl_setqos_class(QosClass::Unspecified);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
pcntl_setqos_class(QosClass::Background);
var_dump(QosClass::Background == pcntl_getqos_class());
?>
--EXPECT--
bool(true)
pcntl_setqos_class(): Argument #1 ($qos_class) must be one of QosClass enum entries : ::UserInteractive, ::UserInitiated, ::Default, ::Utility or ::Background
bool(true)
