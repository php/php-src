--TEST--
GH-25305 (NULL dereference in lxb_dom_document_destroy on lexbor document init failure)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip Zend MM disabled");
?>
--INI--
memory_limit=2M
--FILE--
<?php
$str = str_repeat("a", (int)(1024 * 1024 * 1.25));
Dom\HTMLDocument::createFromString("<p>x</p>");
?>
--EXPECTF--
Fatal error: Allowed memory size of 2097152 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d%A
