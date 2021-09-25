--TEST--
Bug #75774 imap_append HeapCorruction
--EXTENSIONS--
imap
--FILE--
<?php

$fn = __DIR__ . DIRECTORY_SEPARATOR . "foo75774";
$var1 = fopen($fn, "w");

try {
    imap_append($var1, "", "", "", "");
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

fclose($var1);
unlink($fn);

?>
--EXPECT--
imap_append(): Argument #1 ($imap) must be of type IMAP\Connection, resource given
