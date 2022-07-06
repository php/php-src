--TEST--
Bug #53241 (stream casting that relies on fdopen/fopencookie fails with 'xb' mode)
--SKIPIF--
<?php
/* unfortunately no standard function does a cast to FILE*, so we need
 * curl to test this */
if (!extension_loaded("curl")) exit("skip curl extension not loaded");
--FILE--
<?php
$fn = __DIR__ . "/test.tmp";
@unlink($fn);
$fh = fopen($fn, 'xb');
$ch = curl_init('http://www.yahoo.com/');
var_dump(curl_setopt($ch, CURLOPT_FILE, $fh));
echo "Done.\n";
--CLEAN--
<?php
$fn = __DIR__ . "/test.tmp";
@unlink($fn);
?>
--EXPECT--
bool(true)
Done.
