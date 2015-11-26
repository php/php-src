--TEST--
This test will test getStatusString method in ZipArchive
--CREDITS--
Ole-Petter Wikene <olepw@redpill-linpro.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php if (!extension_loaded("zip")) { echo "skip extension not available"; } ?>
--FILE--
<?php

$dirname = dirname(__FILE__) . '/';
$arch = new ZipArchive;
$arch->open($dirname.'foo.zip',ZIPARCHIVE::CREATE);
var_dump($arch->getStatusString());
//delete an index that does not exist - trigger error
$arch->deleteIndex(2);
var_dump($arch->getStatusString());
$arch->close();

?>
--CLEAN--
<?php
unlink($dirname.'foo.zip');
?>
--EXPECT--
string(8) "No error"
string(16) "Invalid argument"

