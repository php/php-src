--TEST--
Test gzdeflate() function : variation
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
    print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
include(__DIR__ . '/data.inc');

echo "*** Testing gzdeflate() : variation ***\n";



echo "\n-- Testing multiple compression --\n";
$output = gzdeflate($data);
var_dump( md5($output));
var_dump(md5(gzdeflate($output)));

?>
--EXPECT--
*** Testing gzdeflate() : variation ***

-- Testing multiple compression --
string(32) "078554fe65e06f6ff01eab51cfc7ae9b"
string(32) "86b9f895ef1377da5269ec3cb2729f71"
