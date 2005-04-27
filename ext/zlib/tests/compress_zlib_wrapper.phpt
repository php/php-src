--TEST--
compress.zlib:// wrapper
--FILE--
<?php

// Relative path
$fp = fopen("compress.zlib://ext/xsl/tests/xslt.xsl.gz", "rb");
fclose($fp);

// Absolute path
$fp = fopen("compress.zlib://". dirname(__FILE__). "/../../../ext/xsl/tests/xslt.xsl.gz", "rb");
fclose($fp);

echo "ok\n";
?>
--EXPECT--
ok
