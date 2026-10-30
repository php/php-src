--TEST--
System ID reinitialization accepts extension entropy after engine restart
--SKIPIF--
<?php
if (ini_get('opcache.file_cache')) die('skip different include behaviour with file_cache');
?>
--EXTENSIONS--
zend_test
--INI--
zend_test.register_system_entropy=1
--PHPDBG--
r
clean
r
clean
r
q
--FILE--
<?php
echo "Done\n";
?>
--EXPECTF--
[Successful compilation of %s]
prompt> Done
[Script ended normally]
prompt> Cleaning Execution Environment
Classes    %d
Functions  %d
Constants  %d
Includes   0
[Script ended normally]
prompt> Done
[Script ended normally]
prompt> Cleaning Execution Environment
Classes    %d
Functions  %d
Constants  %d
Includes   0
[Script ended normally]
prompt> Done
[Script ended normally]
prompt> 
