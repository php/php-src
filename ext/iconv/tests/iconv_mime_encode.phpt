--TEST--
iconv_mime_encode()
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--INI--
iconv.internal_charset=iso-8859-1
--FILE--
<?php
function my_error_handler($errno, $errmsg, $filename, $linenum, $vars)
{
	echo "$errno: $errmsg\n";
}
set_error_handler('my_error_handler');
for ($line_len= 0; $line_len < 80; ++$line_len) {
	print "-------- line length=$line_len\n";
	$result = iconv_mime_encode("From", "サンプル文字列サンプル文字列日本語テキスト", "B", "ISO-2022-JP", "EUC-JP", $line_len, "\n");
	var_dump($result);
	if ($result !== false) {
		list($max) = rsort(array_map("strlen", explode("\n", $result)));
		print "-------- ";
		var_dump(($max <= $line_len));
	} else {
		print "-------- \n";
	}
}
?>
--EXPECTF--
-------- line length=0
2: %s
bool(false)
-------- 
-------- line length=1
2: %s
bool(false)
-------- 
-------- line length=2
2: %s
bool(false)
-------- 
-------- line length=3
2: %s
bool(false)
-------- 
-------- line length=4
2: %s
bool(false)
-------- 
-------- line length=5
2: %s
bool(false)
-------- 
-------- line length=6
2: %s
bool(false)
-------- 
-------- line length=7
2: %s
bool(false)
-------- 
-------- line length=8
2: %s
bool(false)
-------- 
-------- line length=9
2: %s
bool(false)
-------- 
-------- line length=10
2: %s
bool(false)
-------- 
-------- line length=11
2: %s
bool(false)
-------- 
-------- line length=12
2: %s
bool(false)
-------- 
-------- line length=13
2: %s
bool(false)
-------- 
-------- line length=14
2: %s
bool(false)
-------- 
-------- line length=15
2: %s
bool(false)
-------- 
-------- line length=16
2: %s
bool(false)
-------- 
-------- line length=17
2: %s
bool(false)
-------- 
-------- line length=18
2: %s
bool(false)
-------- 
-------- line length=19
2: %s
bool(false)
-------- 
-------- line length=20
2: %s
bool(false)
-------- 
-------- line length=21
2: %s
bool(false)
-------- 
-------- line length=22
2: %s
bool(false)
-------- 
-------- line length=23
2: %s
bool(false)
-------- 
-------- line length=24
2: %s
bool(false)
-------- 
-------- line length=25
2: %s
bool(false)
-------- 
-------- line length=26
2: %s
bool(false)
-------- 
-------- line length=27
2: %s
bool(false)
-------- 
-------- line length=28
2: %s
bool(false)
-------- 
-------- line length=29
2: %s
bool(false)
-------- 
-------- line length=30
2: %s
bool(false)
-------- 
-------- line length=31
2: %s
bool(false)
-------- 
-------- line length=32
2: %s
bool(false)
-------- 
-------- line length=33
2: %s
bool(false)
-------- 
-------- line length=34
2: %s
bool(false)
-------- 
-------- line length=35
2: %s
bool(false)
-------- 
-------- line length=36
string(216) "From: =?ISO-2022-JP?B?GyRCJTUbKEI=?=
 =?ISO-2022-JP?B?JXMlVyVrSjgbKEI=?=
 =?ISO-2022-JP?B?O3pOcyU1JXMbKEI=?=
 =?ISO-2022-JP?B?JVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?TnNGfEtcOGwbKEI=?=
 =?ISO-2022-JP?B?JUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=37
string(216) "From: =?ISO-2022-JP?B?GyRCJTUbKEI=?=
 =?ISO-2022-JP?B?JXMlVyVrSjgbKEI=?=
 =?ISO-2022-JP?B?O3pOcyU1JXMbKEI=?=
 =?ISO-2022-JP?B?JVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?TnNGfEtcOGwbKEI=?=
 =?ISO-2022-JP?B?JUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=38
string(216) "From: =?ISO-2022-JP?B?GyRCJTUbKEI=?=
 =?ISO-2022-JP?B?JXMlVyVrSjgbKEI=?=
 =?ISO-2022-JP?B?O3pOcyU1JXMbKEI=?=
 =?ISO-2022-JP?B?JVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?TnNGfEtcOGwbKEI=?=
 =?ISO-2022-JP?B?JUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=39
string(196) "From: =?ISO-2022-JP?B?GyRCJTUbKEI=?=
 =?ISO-2022-JP?B?JXMlVyVrSjg7ehsoQg==?=
 =?ISO-2022-JP?B?TnMlNSVzJVclaxsoQg==?=
 =?ISO-2022-JP?B?Sjg7ek5zRnxLXBsoQg==?=
 =?ISO-2022-JP?B?OGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=40
string(196) "From: =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?JVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?JTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?O3pOc0Z8S1w4bBsoQg==?=
 =?ISO-2022-JP?B?JUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=41
string(196) "From: =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?JVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?JTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?O3pOc0Z8S1w4bBsoQg==?=
 =?ISO-2022-JP?B?JUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=42
string(196) "From: =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?JVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?JTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?O3pOc0Z8S1w4bBsoQg==?=
 =?ISO-2022-JP?B?JUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=43
string(168) "From: =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?JVcla0o4O3pOcyU1JXMbKEI=?=
 =?ISO-2022-JP?B?JVcla0o4O3pOc0Z8S1wbKEI=?=
 =?ISO-2022-JP?B?OGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=44
string(164) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?Sjg7ek5zJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?Sjg7ek5zRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?JS0lOSVIGyhC?="
-------- bool(true)
-------- line length=45
string(164) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?Sjg7ek5zJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?Sjg7ek5zRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?JS0lOSVIGyhC?="
-------- bool(true)
-------- line length=46
string(164) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?Sjg7ek5zJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?Sjg7ek5zRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?JS0lOSVIGyhC?="
-------- bool(true)
-------- line length=47
string(168) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?Sjg7ek5zJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?O3pOc0Z8S1w4bCVGJS0lORsoQg==?=
 =?ISO-2022-JP?B?JUgbKEI=?="
-------- bool(true)
-------- line length=48
string(144) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?O3pOcyU1JXMlVyVrSjg7ehsoQg==?=
 =?ISO-2022-JP?B?TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=49
string(144) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?O3pOcyU1JXMlVyVrSjg7ehsoQg==?=
 =?ISO-2022-JP?B?TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=50
string(144) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?O3pOcyU1JXMlVyVrSjg7ehsoQg==?=
 =?ISO-2022-JP?B?TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=51
string(140) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?O3pOcyU1JXMlVyVrSjg7ek5zRnwbKEI=?=
 =?ISO-2022-JP?B?S1w4bCVGJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=52
string(140) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?JTUlcyVXJWtKODt6TnNGfEtcOGwbKEI=?=
 =?ISO-2022-JP?B?JUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=53
string(140) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?JTUlcyVXJWtKODt6TnNGfEtcOGwbKEI=?=
 =?ISO-2022-JP?B?JUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=54
string(140) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?JTUlcyVXJWtKODt6TnNGfEtcOGwbKEI=?=
 =?ISO-2022-JP?B?JUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=55
string(140) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?JTUlcyVXJWtKODt6TnNGfEtcOGwlRhsoQg==?=
 =?ISO-2022-JP?B?JS0lOSVIGyhC?="
-------- bool(true)
-------- line length=56
string(144) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNRsoQg==?=
 =?ISO-2022-JP?B?JXMlVyVrSjg7ek5zRnxLXDhsJUYlLRsoQg==?=
 =?ISO-2022-JP?B?JTklSBsoQg==?="
-------- bool(true)
-------- line length=57
string(144) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNRsoQg==?=
 =?ISO-2022-JP?B?JXMlVyVrSjg7ek5zRnxLXDhsJUYlLRsoQg==?=
 =?ISO-2022-JP?B?JTklSBsoQg==?="
-------- bool(true)
-------- line length=58
string(144) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNRsoQg==?=
 =?ISO-2022-JP?B?JXMlVyVrSjg7ek5zRnxLXDhsJUYlLRsoQg==?=
 =?ISO-2022-JP?B?JTklSBsoQg==?="
-------- bool(true)
-------- line length=59
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNRsoQg==?=
 =?ISO-2022-JP?B?JXMlVyVrSjg7ek5zRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=60
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?JWtKODt6TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=61
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?JWtKODt6TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=62
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?JWtKODt6TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=63
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?JWtKODt6TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=64
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVclaxsoQg==?=
 =?ISO-2022-JP?B?Sjg7ek5zRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=65
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVclaxsoQg==?=
 =?ISO-2022-JP?B?Sjg7ek5zRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=66
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVclaxsoQg==?=
 =?ISO-2022-JP?B?Sjg7ek5zRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=67
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVclaxsoQg==?=
 =?ISO-2022-JP?B?Sjg7ek5zRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=68
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=69
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=70
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=71
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=72
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?RnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=73
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?RnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=74
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?RnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=75
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?RnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=76
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOc0Z8S1wbKEI=?=
 =?ISO-2022-JP?B?OGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=77
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOc0Z8S1wbKEI=?=
 =?ISO-2022-JP?B?OGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=78
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOc0Z8S1wbKEI=?=
 =?ISO-2022-JP?B?OGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=79
string(116) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOc0Z8S1wbKEI=?=
 =?ISO-2022-JP?B?OGwlRiUtJTklSBsoQg==?="
-------- bool(true)

