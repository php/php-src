--TEST--
Bug #46944 (json_encode() doesn't handle 3 byte utf8 correctly)
--SKIPIF--
<?php if (!extension_loaded('json')) print 'skip'; ?>
--FILE--
<?php

for ($i = 1; $i <= 16; $i++) {
	$first = 0xf0 | ($i >> 2);
	$second = 0x8f | ($i & 3) << 4;
	$string = sprintf("aa%c%c\xbf\xbdzz", $first, $second);
	echo json_encode($string) . "\n";
}


echo "Done\n";
?>
--EXPECT--
"aa\ud83f\udffdzz"
"aa\ud87f\udffdzz"
"aa\ud8bf\udffdzz"
"aa\ud8ff\udffdzz"
"aa\ud93f\udffdzz"
"aa\ud97f\udffdzz"
"aa\ud9bf\udffdzz"
"aa\ud9ff\udffdzz"
"aa\uda3f\udffdzz"
"aa\uda7f\udffdzz"
"aa\udabf\udffdzz"
"aa\udaff\udffdzz"
"aa\udb3f\udffdzz"
"aa\udb7f\udffdzz"
"aa\udbbf\udffdzz"
"aa\udbff\udffdzz"
Done
