--TEST--
Test bug #65184 strftime() returns insufficient-length string under multibyte locales
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) != 'WIN') {
    die("skip Test is valid for Windows");
}
?>
--FILE--
<?php
	setlocale(LC_ALL, 'Japanese_Japan.932');
	$s = strftime('%A');

	for ($i = 0; $i < strlen($s); $i++) {
		printf("%x ", ord($s[$i]));
	}
	echo "\n";
?>
===DONE===
--EXPECT--
90 85 97 6a 93 fa 
===DONE===
