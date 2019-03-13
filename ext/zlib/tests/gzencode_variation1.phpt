--TEST--
Test gzencode() function : variation
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

include(dirname(__FILE__) . '/data.inc');
include(dirname(__FILE__) . '/gzoscode_util.inc');

echo "*** Testing gzencode() : variation ***\n";

echo "\n-- Testing multiple compression --\n";

$output = gzencode($data);

$gzhex = bin2hex(gzencode($output));
$oscode = detect_oscode();

$expect = $expect_gzhex[$oscode];
$actual = $gzhex;
if ($expect == $actual) {
    var_dump('ok');
} else {
    var_dump([
        'expect' => $expect,
        'actual' => $actual,
    ]);
}

?>
===Done===
--EXPECT--
*** Testing gzencode() : variation ***

-- Testing multiple compression --
string(2) "ok"
===Done===
