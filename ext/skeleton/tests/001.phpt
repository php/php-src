--TEST--
Check if %EXTNAME% is loaded
--EXTENSIONS--
%EXTNAME%
--FILE--
<?php
echo 'The extension "%EXTNAME%" is available';
?>
--EXPECT--
The extension "%EXTNAME%" is available
