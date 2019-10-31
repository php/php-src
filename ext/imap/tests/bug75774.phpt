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

try {
	imap_append($var1, "", "", "", "");
} catch (Throwable $e) {
	echo "\nException: " . $e->getMessage() . "\n";
}

fclose($var1);
unlink($fn);

?>
--EXPECTF--
Warning: imap_append(): internal date not correctly formatted in %s on line %d

Exception: imap_append(): supplied resource is not a valid imap resource
