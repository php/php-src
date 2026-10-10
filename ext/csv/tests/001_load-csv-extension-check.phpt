--TEST--
Check if csv is loaded
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "csv" is available';
?>
--EXPECT--
The extension "csv" is available
