--TEST--
Bug #70601 (Segfault in gc_remove_from_buffer())
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.protect_memory=1
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache')) die("skip"); ?>
--FILE--
<?php
const REPLACEMENTS_PASS_1 = [ "\x00" => "" ];
class a {
	const REPLACEMENTS_PASS_1 = [ "\x01" => "" ];
	public function __construct() {
		echo strtr("Ok",  REPLACEMENTS_PASS_1);
		echo strtr("ay", self::REPLACEMENTS_PASS_1);
	}
}

new a();
?>
--EXPECT--
Okay
