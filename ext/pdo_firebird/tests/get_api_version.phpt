--TEST--
PDO_Firebird: getApiVersion()
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
echo Pdo\Firebird::getApiVersion() . "\n";
echo 'done!';
?>
--EXPECTF--
%d
done!
