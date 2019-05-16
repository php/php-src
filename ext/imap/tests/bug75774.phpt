--TEST--
Bug #75774 imap_append HeapCorruction
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
?>
--FILE--
<?php

$fn = __DIR__ . DIRECTORY_SEPARATOR . "foo75774";
$var1=fopen($fn, "w");

imap_append($var1, "", "", "", "");

fclose($var1);
unlink($fn);

?>
==DONE==
--EXPECTF--
Warning: imap_append(): internal date not correctly formatted in %s on line %d

Warning: imap_append(): supplied resource is not a valid imap resource in %s on line %d
==DONE==
