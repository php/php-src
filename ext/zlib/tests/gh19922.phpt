--TEST--
GH-19922 (gzopen double free on debug build and unseekable stream)
--EXTENSIONS--
zlib
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") !== "0") die("skip Zend MM enabled");
?>
--FILE--
<?php
var_dump(gzopen("php://output", 14));
?>
--EXPECTF--

Warning: gzopen(php://output): could not make seekable - php://output in %s on line %d
bool(false)
