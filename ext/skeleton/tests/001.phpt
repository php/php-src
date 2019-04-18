--TEST--
Check if %EXTNAME% is loaded
--SKIPIF--
<?php
if (!extension_loaded('%EXTNAME%')) {
	echo 'skip';
}
?>
--FILE--
<?php declare(strict_types=1);
echo 'The extension "%EXTNAME%" is available';
?>
--EXPECT--
The extension "%EXTNAME%" is available
