--TEST--
Bug #78185: file cache only no longer works
--INI--
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_cache={PWD}
opcache.file_cache_only=1
--FILE--
<?php
foreach (glob(__DIR__ . '/*/' . __DIR__ . '/*.bin') as $p) {
	var_dump($p);
}
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/*/' . __DIR__ . '/*.bin') as $p) {
	unlink($p);
}
?>
--EXPECTF--
string(%d) "%s78185.php.bin"

