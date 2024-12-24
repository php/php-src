--TEST--
GH-17223 (Memory leak in libxml encoding handling)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument(str_repeat(chr(223), 65537) . str_repeat(chr(8), 17) . str_repeat(chr(133), 257), str_repeat(chr(62), 257));
$doc->save("%00");
?>
--EXPECTF--
Warning: DOMDocument::save(): URI must not contain percent-encoded NUL bytes in %s on line %d
