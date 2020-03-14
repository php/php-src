--TEST--
Bug #71127 (Define in auto_prepend_file is overwrite)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x7FFFBFFF
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache')) die("skip"); ?>
--FILE--
<?php
$file = __DIR__ . "/bug71127.inc";

require_once __DIR__ . '/../../../Zend/tests/constants_helpers.inc';

file_put_contents($file, "<?php tchelper_define('FOO', 'bad'); echo FOO;?>");

tchelper_define("FOO", "okey");

include($file);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug71127.inc");
?>
--EXPECT--
>> define("FOO", string);
true

>> define("FOO", string);
ValueError :: Constant FOO already defined

okey
