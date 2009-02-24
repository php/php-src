--TEST--
Unicode standard conformance test (ill-formed UTF sequences.)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
echo "UTF-8 redundancy\n";
var_dump(bin2hex(mb_convert_encoding(b"\x31\x32\x33", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\x41\x42\x43", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xc0\xb1\xc0\xb2\xc0\xb3", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xc1\x81\xc1\x82\xc1\x83", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xe0\x80\xb1\xe0\x80\xb2\xe0\x80\xb3", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xe0\x81\x81\xe0\x81\x82\xe0\x81\x83", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf0\x80\x80\xb1\xf0\x80\x80\xb2\xf0\x80\x80\xb3", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf0\x80\x81\x81\xf0\x80\x81\x82\xf0\x81\x83", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf8\x80\x80\x80\xb1\xf8\x80\x80\x80\xb2\xf8\x80\x80\x80\xb3", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf8\x80\x80\x81\x81\xf8\x80\x80\x81\x82\xf8\x80\x80\x81\x83", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xfc\x80\x80\x80\x80\xb1\xfc\x80\x80\x80\x80\xb2\xfc\x80\x80\x80\x80\xb3", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xfc\x80\x80\x80\x81\x81\xfc\x80\x80\x80\x81\x82\xfc\x80\x80\x80\x81\x83", "UCS-4BE", "UTF-8")));

var_dump(bin2hex(mb_convert_encoding(b"\xc2\xa2\xc2\xa3\xc2\xa5", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xe0\x82\xa2\xe0\x82\xa3\xe0\x82\xa5", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf0\x80\x82\xa2\xf0\x80\x82\xa3\xf0\x80\x82\xa5", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf8\x80\x80\x82\xa2\xf8\x80\x80\x82\xa3\xf8\x80\x80\x82\xa5", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xfc\x80\x80\x80\x82\xa2\xfc\x80\x80\x80\x82\xa3\xfc\x80\x80\x80\x82\xa5", "UCS-4BE", "UTF-8")));

var_dump(bin2hex(mb_convert_encoding(b"\xc1\xbf", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xc2\x80", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xdf\xbf", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xe0\x9f\xff", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xe0\xa0\x80", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xef\xbf\xbf", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf0\x8f\xbf\xbf", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf0\x90\x80\x80", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf7\xbf\xbf\xbf", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf8\x87\xbf\xbf\xbf", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xf8\x88\x80\x80\x80", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xfb\xbf\xbf\xbf\xbf", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xfc\x83\xbf\xbf\xbf\xbf", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xfc\x84\x80\x80\x80\x80", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xfd\xaf\xbf\xbf\xbf\xbf", "UCS-4BE", "UTF-8")));
var_dump(bin2hex(mb_convert_encoding(b"\xfd\xbf\xbf\xbf\xbf\xbf", "UCS-4BE", "UTF-8")));

echo "UTF-8 and surrogates area\n";
$out = b'';
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $out .= mb_convert_encoding(pack('C3', 0xe0 | ($i >> 12), 0x80 | ($i >> 6) & 0x3f, 0x80 | $i & 0x3f), "UCS-4BE", "UTF-8");
}
var_dump(bin2hex($out));

echo "UTF-32 code range\n";
var_dump(bin2hex(mb_convert_encoding("\x00\x11\x00\x00", "UCS-4BE", "UTF-32BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x10\xff\xff", "UCS-4BE", "UTF-32BE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\x11\x00", "UCS-4BE", "UTF-32LE")));
var_dump(bin2hex(mb_convert_encoding("\xff\xff\x10\x00", "UCS-4BE", "UTF-32LE")));
var_dump(bin2hex(mb_convert_encoding("\x00\x11\x00\x00", "UCS-4BE", "UTF-32")));
var_dump(bin2hex(mb_convert_encoding("\x00\x10\xff\xff", "UCS-4BE", "UTF-32")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\xfe\xff\x00\x11\x00\x00", "UCS-4BE", "UTF-32")));
var_dump(bin2hex(mb_convert_encoding("\x00\x00\xfe\xff\x00\x10\xff\xff", "UCS-4BE", "UTF-32")));
var_dump(bin2hex(mb_convert_encoding("\xff\xfe\x00\x00\x00\x00\x11\x00", "UCS-4BE", "UTF-32")));
var_dump(bin2hex(mb_convert_encoding("\xff\xfe\x00\x00\xff\xff\x10\x00", "UCS-4BE", "UTF-32")));

echo "UTF-32 and surrogates area\n";
$out = b'';
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $out .= mb_convert_encoding(pack('C4', $i >> 24, ($i >> 16) & 0xff, ($i >> 8) & 0xff, $i & 0xff), "UCS-4BE", "UTF-32BE");
}
var_dump(bin2hex($out));

$out = b'';
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $out .= mb_convert_encoding(pack('C4', $i & 0xff, ($i >> 8) & 0xff, ($i >> 16) & 0xff, ($i >> 24) & 0xff), "UCS-4BE", "UTF-32LE");
}
var_dump(bin2hex($out));

$out = b'';
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $out .= mb_convert_encoding(pack('C4', $i >> 24, ($i >> 16) & 0xff, ($i >> 8) & 0xff, $i & 0xff), "UCS-4BE", "UTF-32");
}
var_dump(bin2hex($out));

$out = b'';
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $out .= mb_convert_encoding("\x00\x00\xfe\xff". pack('C4', $i >> 24, ($i >> 16) & 0xff, ($i >> 8) & 0xff, $i & 0xff), "UCS-4BE", "UTF-32");
}
var_dump(str_replace("0000feff", "", bin2hex($out)));

$out = b'';
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $out .= mb_convert_encoding("\xff\xfe\x00\x00". pack('C4', $i & 0xff, ($i >> 8) & 0xff, ($i >> 16) & 0xff, ($i >> 24) & 0xff), "UCS-4BE", "UTF-32");
}
var_dump(str_replace("0000feff", "", bin2hex($out)));
?>
--EXPECT--
UTF-8 redundancy
unicode(24) "000000310000003200000033"
unicode(24) "000000410000004200000043"
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(24) "000000a2000000a3000000a5"
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(0) ""
unicode(8) "00000080"
unicode(8) "000007ff"
unicode(0) ""
unicode(8) "00000800"
unicode(8) "0000ffff"
unicode(0) ""
unicode(8) "00010000"
unicode(8) "001fffff"
unicode(0) ""
unicode(8) "00200000"
unicode(8) "03ffffff"
unicode(0) ""
unicode(8) "04000000"
unicode(8) "6fffffff"
unicode(0) ""
UTF-8 and surrogates area
unicode(16) "0000d7ff0000e000"
UTF-32 code range
unicode(0) ""
unicode(8) "0010ffff"
unicode(0) ""
unicode(8) "0010ffff"
unicode(0) ""
unicode(8) "0010ffff"
unicode(8) "0000feff"
unicode(16) "0000feff0010ffff"
unicode(8) "0000feff"
unicode(16) "0000feff0010ffff"
UTF-32 and surrogates area
unicode(16) "0000d7ff0000e000"
unicode(16) "0000d7ff0000e000"
unicode(16) "0000d7ff0000e000"
unicode(16) "0000d7ff0000e000"
unicode(16) "0000d7ff0000e000"
