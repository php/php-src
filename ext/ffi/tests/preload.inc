<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
	$fn = __DIR__ . "/300-win32.h";
	$cont = str_replace(
			"PHP_DLL_NAME",
			"php" . PHP_MAJOR_VERSION . (PHP_ZTS ? "ts" : "") . (PHP_DEBUG ? "_debug" : "") . ".dll",
			file_get_contents("$fn.in")
		);
	file_put_contents($fn, $cont);
	$ffi = FFI::load($fn);
	/* Test should cleanup this. */
} else {
	FFI::load(__DIR__ . "/300.h");
}
