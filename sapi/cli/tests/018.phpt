--TEST--
CLI php -m 
--SKIPIF--
<?php 
include "skipif.inc"; 
if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');


echo `"$php" -n -m`;

echo "Done\n";
?>
--EXPECTF--     
[PHP Modules]
%a
pcre
%a

[Zend Modules]
%aDone
