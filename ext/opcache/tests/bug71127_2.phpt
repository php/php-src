--TEST--
Bug #71127_2 (Define in auto_prepend_file is overwrite)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x7FFFFFFF
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache')) die("skip"); ?>
--FILE--
<?php
$file = __DIR__ . "/bug71127.inc";

file_put_contents($file, <<<'EOF'
<?php
try {
	define('FOO', 'bad');
} catch (Throwable $e) {
	echo "Exception: " . $e->getMessage() . "\n";
}
echo FOO;
?>
EOF
);

define("FOO", "okey");

include($file);
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug71127.inc");
?>
--EXPECT--
Exception: Constant FOO already defined
okey
