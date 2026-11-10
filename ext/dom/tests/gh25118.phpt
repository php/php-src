--TEST--
GH-25118 (Dom\HTMLDocument::createFromString() crashes on OOM during conversion)
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip 64-bit only");
?>
--INI--
memory_limit=32M
--FILE--
<?php
$html = '<template><p>t</p><template><b>n</b></template></template><main>' . str_repeat('<i></i>', 100000) . '</main>';
Dom\HTMLDocument::createFromString($html, LIBXML_NOERROR);
?>
--EXPECTF--
Fatal error: Allowed memory size of 33554432 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
