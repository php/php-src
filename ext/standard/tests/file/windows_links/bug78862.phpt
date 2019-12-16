--TEST--
Bug #78862 (link() silently truncates after a null byte on Windows)
--FILE--
<?php
file_put_contents(__DIR__ . '/bug78862.target', 'foo');
var_dump(link(__DIR__ . "/bug78862.target\0more", __DIR__ . "/bug78862.link\0more"));
var_dump(file_exists(__DIR__ . '/bug78862.link'));
?>
--EXPECTF--
Warning: link() expects parameter 1 to be a valid path, string given in %s on line %d
NULL
bool(false)
--CLEAN--
<?php
unlink(__DIR__ . '/bug78862.target');
unlink(__DIR__ . '/bug78862.link');
?>
