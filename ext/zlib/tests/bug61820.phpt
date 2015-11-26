--TEST--
bug #61820 using ob_gzhandler will complain about headers already sent when no compression
--SKIPIF--
<?php
extension_loaded("zlib") or die("skip");
?>
--FILE--
<?php
ob_start('ob_gzhandler');

echo "Hi there.\n";
ob_flush();
flush();

echo "This is confusing...\n";
ob_flush();
flush();
?>
DONE
--EXPECT--
Hi there.
This is confusing...
DONE
