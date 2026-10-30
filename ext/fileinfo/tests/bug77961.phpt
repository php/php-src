--TEST--
Bug #77961 (finfo_open crafted magic parsing SIGABRT)
--EXTENSIONS--
fileinfo
--FILE--
<?php
finfo_open(FILEINFO_NONE, __DIR__ . '/bug77961.magic');
?>
--EXPECTF--
Warning: finfo_open(): Expected numeric type got `indirect' in %s on line %d

Warning: finfo_open(): Failed to load magic database at "%sbug77961.magic" in %s on line %d
