--TEST--
bug #61443
--SKIPIF--
<?php
extension_loaded("zlib") or die("skip");
?>
--FILE--
<?php
ob_start(); echo "foo\n"; ob_get_clean(); 
if(!headers_sent()) ini_set('zlib.output_compression', true); echo "end\n";
?>
DONE
--EXPECTF--
end
DONE
