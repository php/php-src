--TEST--
Bug #61139 (gzopen leaks when specifying invalid mode)
--SKIPIF--
<?php
if (!extension_loaded('zlib')) {
	die('skip - zlib extension not loaded');
}
?>
--FILE--
<?php

gzopen('someFile', 'c');
--CLEAN--
<?php
	unlink('someFile');
?>
--EXPECTF--
Warning: gzopen(): gzopen failed in %s on line %d
