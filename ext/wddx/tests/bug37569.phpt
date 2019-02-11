--TEST--
Bug #37569 (WDDX incorrectly encodes high-ascii characters)
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php
for ($i = 65; $i < 256; $i++) {
	if ($i >= 0xc0) {
		$v = chr(0xc3) . chr($i - 64);
	} elseif ($i >= 0x80) {
		$v = chr(0xc2) . chr($i);
	} else {
		$v = chr($i); // make it UTF-8
	}
	$ret = wddx_serialize_value($v);
	echo $ret . "\n";
	var_dump(bin2hex($v), bin2hex(wddx_deserialize($ret)), $v == wddx_deserialize($ret));
}
?>
--EXPECTF--
Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>A</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "41"
string(2) "41"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>B</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "42"
string(2) "42"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>C</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "43"
string(2) "43"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>D</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "44"
string(2) "44"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>E</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "45"
string(2) "45"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>F</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "46"
string(2) "46"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>G</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "47"
string(2) "47"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>H</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "48"
string(2) "48"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>I</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "49"
string(2) "49"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>J</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "4a"
string(2) "4a"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>K</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "4b"
string(2) "4b"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>L</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "4c"
string(2) "4c"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>M</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "4d"
string(2) "4d"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>N</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "4e"
string(2) "4e"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>O</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "4f"
string(2) "4f"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>P</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "50"
string(2) "50"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Q</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "51"
string(2) "51"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>R</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "52"
string(2) "52"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>S</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "53"
string(2) "53"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>T</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "54"
string(2) "54"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>U</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "55"
string(2) "55"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>V</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "56"
string(2) "56"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>W</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "57"
string(2) "57"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>X</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "58"
string(2) "58"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Y</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "59"
string(2) "59"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Z</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "5a"
string(2) "5a"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>[</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "5b"
string(2) "5b"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>\</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "5c"
string(2) "5c"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>]</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "5d"
string(2) "5d"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>^</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "5e"
string(2) "5e"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>_</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "5f"
string(2) "5f"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>`</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "60"
string(2) "60"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>a</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "61"
string(2) "61"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>b</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "62"
string(2) "62"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>c</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "63"
string(2) "63"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>d</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "64"
string(2) "64"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>e</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "65"
string(2) "65"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>f</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "66"
string(2) "66"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>g</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "67"
string(2) "67"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>h</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "68"
string(2) "68"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>i</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "69"
string(2) "69"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>j</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "6a"
string(2) "6a"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>k</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "6b"
string(2) "6b"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>l</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "6c"
string(2) "6c"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>m</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "6d"
string(2) "6d"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>n</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "6e"
string(2) "6e"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>o</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "6f"
string(2) "6f"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>p</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "70"
string(2) "70"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>q</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "71"
string(2) "71"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>r</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "72"
string(2) "72"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>s</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "73"
string(2) "73"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>t</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "74"
string(2) "74"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>u</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "75"
string(2) "75"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>v</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "76"
string(2) "76"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>w</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "77"
string(2) "77"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>x</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "78"
string(2) "78"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>y</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "79"
string(2) "79"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>z</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "7a"
string(2) "7a"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>{</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "7b"
string(2) "7b"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>|</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "7c"
string(2) "7c"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>}</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "7d"
string(2) "7d"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>~</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "7e"
string(2) "7e"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(2) "7f"
string(2) "7f"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c280"
string(4) "c280"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c281"
string(4) "c281"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c282"
string(4) "c282"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c283"
string(4) "c283"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c284"
string(4) "c284"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c285"
string(4) "c285"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c286"
string(4) "c286"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c287"
string(4) "c287"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c288"
string(4) "c288"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c289"
string(4) "c289"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c28a"
string(4) "c28a"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c28b"
string(4) "c28b"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c28c"
string(4) "c28c"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c28d"
string(4) "c28d"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c28e"
string(4) "c28e"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c28f"
string(4) "c28f"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c290"
string(4) "c290"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c291"
string(4) "c291"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c292"
string(4) "c292"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c293"
string(4) "c293"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c294"
string(4) "c294"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c295"
string(4) "c295"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c296"
string(4) "c296"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c297"
string(4) "c297"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c298"
string(4) "c298"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c299"
string(4) "c299"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c29a"
string(4) "c29a"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c29b"
string(4) "c29b"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c29c"
string(4) "c29c"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c29d"
string(4) "c29d"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c29e"
string(4) "c29e"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string></string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c29f"
string(4) "c29f"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string> </string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a0"
string(4) "c2a0"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¡</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a1"
string(4) "c2a1"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¢</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a2"
string(4) "c2a2"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>£</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a3"
string(4) "c2a3"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¤</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a4"
string(4) "c2a4"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¥</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a5"
string(4) "c2a5"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¦</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a6"
string(4) "c2a6"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>§</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a7"
string(4) "c2a7"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¨</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a8"
string(4) "c2a8"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>©</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2a9"
string(4) "c2a9"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ª</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2aa"
string(4) "c2aa"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>«</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2ab"
string(4) "c2ab"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¬</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2ac"
string(4) "c2ac"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>­</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2ad"
string(4) "c2ad"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>®</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2ae"
string(4) "c2ae"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¯</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2af"
string(4) "c2af"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>°</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b0"
string(4) "c2b0"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>±</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b1"
string(4) "c2b1"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>²</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b2"
string(4) "c2b2"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>³</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b3"
string(4) "c2b3"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>´</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b4"
string(4) "c2b4"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>µ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b5"
string(4) "c2b5"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¶</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b6"
string(4) "c2b6"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>·</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b7"
string(4) "c2b7"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¸</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b8"
string(4) "c2b8"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¹</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2b9"
string(4) "c2b9"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>º</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2ba"
string(4) "c2ba"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>»</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2bb"
string(4) "c2bb"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¼</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2bc"
string(4) "c2bc"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>½</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2bd"
string(4) "c2bd"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¾</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2be"
string(4) "c2be"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>¿</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c2bf"
string(4) "c2bf"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>À</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c380"
string(4) "c380"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Á</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c381"
string(4) "c381"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Â</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c382"
string(4) "c382"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ã</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c383"
string(4) "c383"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ä</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c384"
string(4) "c384"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Å</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c385"
string(4) "c385"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Æ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c386"
string(4) "c386"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ç</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c387"
string(4) "c387"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>È</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c388"
string(4) "c388"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>É</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c389"
string(4) "c389"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ê</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c38a"
string(4) "c38a"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ë</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c38b"
string(4) "c38b"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ì</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c38c"
string(4) "c38c"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Í</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c38d"
string(4) "c38d"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Î</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c38e"
string(4) "c38e"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ï</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c38f"
string(4) "c38f"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ð</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c390"
string(4) "c390"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ñ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c391"
string(4) "c391"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ò</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c392"
string(4) "c392"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ó</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c393"
string(4) "c393"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ô</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c394"
string(4) "c394"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Õ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c395"
string(4) "c395"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ö</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c396"
string(4) "c396"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>×</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c397"
string(4) "c397"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ø</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c398"
string(4) "c398"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ù</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c399"
string(4) "c399"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ú</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c39a"
string(4) "c39a"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Û</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c39b"
string(4) "c39b"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ü</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c39c"
string(4) "c39c"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Ý</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c39d"
string(4) "c39d"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>Þ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c39e"
string(4) "c39e"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ß</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c39f"
string(4) "c39f"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>à</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a0"
string(4) "c3a0"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>á</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a1"
string(4) "c3a1"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>â</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a2"
string(4) "c3a2"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ã</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a3"
string(4) "c3a3"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ä</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a4"
string(4) "c3a4"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>å</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a5"
string(4) "c3a5"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>æ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a6"
string(4) "c3a6"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ç</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a7"
string(4) "c3a7"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>è</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a8"
string(4) "c3a8"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>é</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3a9"
string(4) "c3a9"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ê</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3aa"
string(4) "c3aa"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ë</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3ab"
string(4) "c3ab"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ì</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3ac"
string(4) "c3ac"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>í</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3ad"
string(4) "c3ad"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>î</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3ae"
string(4) "c3ae"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ï</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3af"
string(4) "c3af"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ð</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b0"
string(4) "c3b0"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ñ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b1"
string(4) "c3b1"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ò</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b2"
string(4) "c3b2"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ó</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b3"
string(4) "c3b3"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ô</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b4"
string(4) "c3b4"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>õ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b5"
string(4) "c3b5"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ö</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b6"
string(4) "c3b6"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>÷</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b7"
string(4) "c3b7"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ø</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b8"
string(4) "c3b8"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ù</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3b9"
string(4) "c3b9"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ú</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3ba"
string(4) "c3ba"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>û</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3bb"
string(4) "c3bb"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ü</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3bc"
string(4) "c3bc"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ý</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3bd"
string(4) "c3bd"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>þ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3be"
string(4) "c3be"
bool(true)

Deprecated: Function wddx_serialize_value() is deprecated in %s on line %d
<wddxPacket version='1.0'><header/><data><string>ÿ</string></data></wddxPacket>

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d

Deprecated: Function wddx_deserialize() is deprecated in %s on line %d
string(4) "c3bf"
string(4) "c3bf"
bool(true)
