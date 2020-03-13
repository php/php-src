--TEST--
FFI 301: FFI loading on Windows
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (substr(PHP_OS, 0, 3) != 'WIN') die('skip for Windows only'); ?>
--INI--
ffi.enable=1
--FILE--
<?php
require_once('utils.inc');
$fn = __DIR__ . "/300-win32.h";
$cont = str_replace(
		"PHP_DLL_NAME",
		ffi_get_php_dll_name(),
		file_get_contents("$fn.in")
	);
file_put_contents($fn, $cont);

$ffi = FFI::load($fn);
$ffi->php_printf("Hello World from %s!\n", "PHP");
?>
--CLEAN--
<?php
	$fn = __DIR__ . "/300-win32.h";
	unlink($fn);
?>
--EXPECT--
Hello World from PHP!
