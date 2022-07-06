--TEST--
Check if %EXTNAME% is loaded
--SKIPIF--
<?php
if (!extension_loaded('%EXTNAME%')) {
    echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "%EXTNAME%" is available';
?>
--EXPECT--
The extension "%EXTNAME%" is available
