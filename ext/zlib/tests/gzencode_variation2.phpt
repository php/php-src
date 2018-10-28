--TEST--
Test gzencode() function : variation - verify header contents with all encoding modes
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : string gzencode  ( string $data  [, int $level  [, int $encoding_mode  ]] )
 * Description: Gzip-compress a string
 * Source code: ext/zlib/zlib.c
 * Alias to functions:
 */

echo "*** Testing gzencode() : variation ***\n";

$data = "A small string to encode\n";

echo "\n-- Testing with each encoding_mode  --\n";

include(dirname(__FILE__) . '/gzoscode_util.inc');

function dump_gzhex_status($gzhex) {
    var_dump(test_oscode($gzhex));
    var_dump(filter_oscode($gzhex, '00'));
}

dump_gzhex_status(bin2hex(gzencode($data, -1)));
dump_gzhex_status(bin2hex(gzencode($data, -1, FORCE_GZIP)));
var_dump(bin2hex(gzencode($data, -1, FORCE_DEFLATE)));

?>
===DONE===
--EXPECT--
*** Testing gzencode() : variation ***

-- Testing with each encoding_mode  --
string(2) "ok"
string(90) "1f8b0800000000000000735428ce4dccc951282e29cacc4b5728c95748cd4bce4f49e50200d7739de519000000"
string(2) "ok"
string(90) "1f8b0800000000000000735428ce4dccc951282e29cacc4b5728c95748cd4bce4f49e50200d7739de519000000"
string(66) "789c735428ce4dccc951282e29cacc4b5728c95748cd4bce4f49e50200735808cd"
===DONE===
