--TEST--
PDO_Firebird: getApiVersion()
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
echo Pdo\Firebird::getApiVersion() . "\n";
echo 'done!';
?>
--EXPECTF--
%d
done!
