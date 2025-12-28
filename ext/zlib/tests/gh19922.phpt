--TEST--
GH-19922 (gzopen double free on debug build and unseekable stream)
--EXTENSIONS--
zlib
--FILE--
<?php
var_dump(gzopen("php://output", 14));
?>
--EXPECTF--

Warning: gzopen(php://output): could not make seekable - php://output in %s on line %d
bool(false)
