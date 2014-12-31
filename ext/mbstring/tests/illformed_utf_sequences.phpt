--TEST--
Unicode standard conformance test (ill-formed UTF sequences.)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
function chk_enc($str, $n, $enc = "UTF-8", $with_bom = false) {
	$src = bin2hex(mb_convert_encoding($str, "UCS-4BE", $enc));
	$dst = str_repeat("0000fffd", $n);
	if ($with_bom) {
		$dst = "0000feff" . $dst;
	}
	if ($dst == $src) {
		return false;
	} else {
		return $src;
	}
}

mb_substitute_character(0xfffd);


echo "UTF-8 redundancy\n";
var_dump(chk_enc("\x31\x32\x33", 0));
var_dump(chk_enc("\x41\x42\x43", 0));
var_dump(chk_enc("\xc0\xb1\xc0\xb2\xc0\xb3", 6));
var_dump(chk_enc("\xc1\x81\xc1\x82\xc1\x83", 6));
var_dump(chk_enc("\xe0\x80\xb1\xe0\x80\xb2\xe0\x80\xb3", 9));
var_dump(chk_enc("\xe0\x81\x81\xe0\x81\x82\xe0\x81\x83", 9));
var_dump(chk_enc("\xf0\x80\x80\xb1\xf0\x80\x80\xb2\xf0\x80\x80\xb3", 12));
var_dump(chk_enc("\xf0\x80\x81\x81\xf0\x80\x81\x82\xf0\x81\x83", 11));
var_dump(chk_enc("\xf8\x80\x80\x80\xb1\xf8\x80\x80\x80\xb2\xf8\x80\x80\x80\xb3", 15));
var_dump(chk_enc("\xf8\x80\x80\x81\x81\xf8\x80\x80\x81\x82\xf8\x80\x80\x81\x83", 15));
var_dump(chk_enc("\xfc\x80\x80\x80\x80\xb1\xfc\x80\x80\x80\x80\xb2\xfc\x80\x80\x80\x80\xb3", 18));
var_dump(chk_enc("\xfc\x80\x80\x80\x81\x81\xfc\x80\x80\x80\x81\x82\xfc\x80\x80\x80\x81\x83", 18));

var_dump(chk_enc("\xc2\xa2\xc2\xa3\xc2\xa5", 0));
var_dump(chk_enc("\xe0\x82\xa2\xe0\x82\xa3\xe0\x82\xa5", 9));
var_dump(chk_enc("\xf0\x80\x82\xa2\xf0\x80\x82\xa3\xf0\x80\x82\xa5", 12));
var_dump(chk_enc("\xf8\x80\x80\x82\xa2\xf8\x80\x80\x82\xa3\xf8\x80\x80\x82\xa5", 15));
var_dump(chk_enc("\xfc\x80\x80\x80\x82\xa2\xfc\x80\x80\x80\x82\xa3\xfc\x80\x80\x80\x82\xa5", 18));

var_dump(chk_enc("\xc1\xbf", 2));
var_dump(chk_enc("\xc2\x80", 0));
var_dump(chk_enc("\xdf\xbf", 0));
var_dump(chk_enc("\xe0\x9f\xff", 3));
var_dump(chk_enc("\xe0\xa0\x80", 2));
var_dump(chk_enc("\xef\xbf\xbf", 0));
var_dump(chk_enc("\xf0\x8f\xbf\xbf", 4));
var_dump(chk_enc("\xf0\x90\x80\x80", 0));
var_dump(chk_enc("\xf7\xbf\xbf\xbf", 4));
var_dump(chk_enc("\xf8\x87\xbf\xbf\xbf", 5));
var_dump(chk_enc("\xf8\x88\x80\x80\x80", 5));
var_dump(chk_enc("\xfb\xbf\xbf\xbf\xbf", 5));
var_dump(chk_enc("\xfc\x83\xbf\xbf\xbf\xbf", 6));
var_dump(chk_enc("\xfc\x84\x80\x80\x80\x80", 6));
var_dump(chk_enc("\xfd\xaf\xbf\xbf\xbf\xbf", 6));
var_dump(chk_enc("\xfd\xbf\xbf\xbf\xbf\xbf", 6));

echo "UTF-8 and surrogates area\n";
$out = '';
$cnt = 0;
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
	$s = chk_enc(pack('C3', 0xe0 | ($i >> 12), 0x80 | ($i >> 6) & 0x3f, 0x80 | $i & 0x3f), 3);
	if ($s === false) {
		$cnt++;
	} else {
		$out .= $s;
	}
}
var_dump($cnt);
var_dump($out);

echo "UTF-32 code range\n";
var_dump(chk_enc("\x00\x11\x00\x00", 1, "UTF-32BE"));
var_dump(chk_enc("\x00\x10\xff\xff", 0, "UTF-32BE"));
var_dump(chk_enc("\x00\x00\x11\x00", 1, "UTF-32LE"));
var_dump(chk_enc("\xff\xff\x10\x00", 0, "UTF-32LE"));
var_dump(chk_enc("\x00\x11\x00\x00", 1, "UTF-32"));
var_dump(chk_enc("\x00\x10\xff\xff", 0, "UTF-32"));
var_dump(chk_enc("\x00\x00\xfe\xff\x00\x11\x00\x00", 0, "UTF-32"));
var_dump(chk_enc("\x00\x00\xfe\xff\x00\x10\xff\xff", 0, "UTF-32"));
var_dump(chk_enc("\xff\xfe\x00\x00\x00\x00\x11\x00", 0, "UTF-32"));
var_dump(chk_enc("\xff\xfe\x00\x00\xff\xff\x10\x00", 0, "UTF-32"));

echo "UTF-32 and surrogates area\n";
$out = '';
$cnt = 0;
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $s = chk_enc(pack('C4', $i >> 24, ($i >> 16) & 0xff, ($i >> 8) & 0xff, $i & 0xff), 1, "UTF-32BE");
	if ($s === false) {
		$cnt++;
	} else {
		$out .= $s;
	}
}
var_dump($cnt);
var_dump($out);

$out = '';
$cnt = 0;
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $s = chk_enc(pack('C4', $i & 0xff, ($i >> 8) & 0xff, ($i >> 16) & 0xff, ($i >> 24) & 0xff), 1, "UTF-32LE");
	if ($s === false) {
		$cnt++;
	} else {
		$out .= $s;
	}
}
var_dump($cnt);
var_dump($out);

$out = '';
$cnt = 0;
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $s = chk_enc(pack('C4', $i >> 24, ($i >> 16) & 0xff, ($i >> 8) & 0xff, $i & 0xff), 1, "UTF-32");
	if ($s === false) {
		$cnt++;
	} else {
		$out .= $s;
	}
}
var_dump($cnt);
var_dump($out);

echo "UTF-32 and surrogates area with BOM\n";

$out = '';
$cnt = 0;
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $s = chk_enc("\x00\x00\xfe\xff". pack('C4', $i >> 24, ($i >> 16) & 0xff, ($i >> 8) & 0xff, $i & 0xff), 
				 1, "UTF-32", true);
	if ($s === false) {
		$cnt++;
	} else {
		$out .= $s;
	}
}
var_dump($cnt);
var_dump(str_replace("0000feff","",$out));

$out = '';
$cnt = 0;
for ($i = 0xd7ff; $i <= 0xe000; ++$i) {
    $s = chk_enc("\xff\xfe\x00\x00". pack('C4', $i & 0xff, ($i >> 8) & 0xff, ($i >> 16) & 0xff, ($i >> 24) & 0xff), 
				 1, "UTF-32", true);
	if ($s === false) {
		$cnt++;
	} else {
		$out .= $s;
	}
}
var_dump($cnt);
var_dump(str_replace("0000feff","",$out));

?>
--EXPECT--
UTF-8 redundancy
string(24) "000000310000003200000033"
string(24) "000000410000004200000043"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
string(24) "000000a2000000a3000000a5"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
string(8) "00000080"
string(8) "000007ff"
bool(false)
string(8) "00000800"
string(8) "0000ffff"
bool(false)
string(8) "00010000"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
UTF-8 and surrogates area
int(2048)
string(16) "0000d7ff0000e000"
UTF-32 code range
bool(false)
string(8) "0010ffff"
bool(false)
string(8) "0010ffff"
bool(false)
string(8) "0010ffff"
string(16) "0000feff0000fffd"
string(16) "0000feff0010ffff"
string(16) "0000feff0000fffd"
string(16) "0000feff0010ffff"
UTF-32 and surrogates area
int(2048)
string(16) "0000d7ff0000e000"
int(2048)
string(16) "0000d7ff0000e000"
int(2048)
string(16) "0000d7ff0000e000"
UTF-32 and surrogates area with BOM
int(2048)
string(16) "0000d7ff0000e000"
int(2048)
string(16) "0000d7ff0000e000"
