--TEST--
compress.zlib:// wrapper
--EXTENSIONS--
zlib
--FILE--
<?php
chdir(__DIR__. "/../../..");

$pfx = str_repeat('../', substr_count($_SERVER['PHP_SELF'], '../'));

// Relative path
$fp = fopen("compress.zlib://{$pfx}ext/xsl/tests/xslt.xsl.gz", "rb");
fclose($fp);

// Absolute path
$fp = fopen("compress.zlib://". __DIR__. "/../../../ext/xsl/tests/xslt.xsl.gz", "rb");
fclose($fp);

echo "ok\n";
?>
--EXPECT--
ok
