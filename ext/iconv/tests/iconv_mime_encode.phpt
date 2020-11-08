--TEST--
iconv_mime_encode()
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--INI--
iconv.internal_charset=iso-8859-1
--FILE--
<?php
function my_error_handler($errno, $errmsg, $filename, $linenum)
{
    echo "$errno: $errmsg\n";
}
set_error_handler('my_error_handler');
$preference = array(
    "scheme" => "B",
    "output-charset" => "ISO-2022-JP",
    "input-charset" => "EUC-JP",
    "line-break-chars" => "\n"
);
for ($line_len= 0; $line_len < 80; ++$line_len) {
    print "-------- line length=$line_len\n";
    $preference["line-length"] = $line_len;
    $result = iconv_mime_encode("From", "サンプル文字列サンプル文字列日本語テキスト", $preference);
    var_dump($result);
    if ($result !== false) {
                $max = max(array_map("strlen", explode("\n", $result)));
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
string(396) "From: =?ISO-2022-JP?B?GyRCJTUbKEI=?=
 =?ISO-2022-JP?B?GyRCJXMlVxsoQg==?=
 =?ISO-2022-JP?B?GyRCJWtKOBsoQg==?=
 =?ISO-2022-JP?B?GyRCO3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVclaxsoQg==?=
 =?ISO-2022-JP?B?GyRCSjg7ehsoQg==?=
 =?ISO-2022-JP?B?GyRCTnNGfBsoQg==?=
 =?ISO-2022-JP?B?GyRCS1w4bBsoQg==?=
 =?ISO-2022-JP?B?GyRCJUYlLRsoQg==?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=37
string(396) "From: =?ISO-2022-JP?B?GyRCJTUbKEI=?=
 =?ISO-2022-JP?B?GyRCJXMlVxsoQg==?=
 =?ISO-2022-JP?B?GyRCJWtKOBsoQg==?=
 =?ISO-2022-JP?B?GyRCO3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVclaxsoQg==?=
 =?ISO-2022-JP?B?GyRCSjg7ehsoQg==?=
 =?ISO-2022-JP?B?GyRCTnNGfBsoQg==?=
 =?ISO-2022-JP?B?GyRCS1w4bBsoQg==?=
 =?ISO-2022-JP?B?GyRCJUYlLRsoQg==?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=38
string(396) "From: =?ISO-2022-JP?B?GyRCJTUbKEI=?=
 =?ISO-2022-JP?B?GyRCJXMlVxsoQg==?=
 =?ISO-2022-JP?B?GyRCJWtKOBsoQg==?=
 =?ISO-2022-JP?B?GyRCO3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVclaxsoQg==?=
 =?ISO-2022-JP?B?GyRCSjg7ehsoQg==?=
 =?ISO-2022-JP?B?GyRCTnNGfBsoQg==?=
 =?ISO-2022-JP?B?GyRCS1w4bBsoQg==?=
 =?ISO-2022-JP?B?GyRCJUYlLRsoQg==?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=39
string(236) "From: =?ISO-2022-JP?B?GyRCJTUbKEI=?=
 =?ISO-2022-JP?B?GyRCJXMlVyVrSjgbKEI=?=
 =?ISO-2022-JP?B?GyRCO3pOcyU1JXMbKEI=?=
 =?ISO-2022-JP?B?GyRCJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?GyRCTnNGfEtcOGwbKEI=?=
 =?ISO-2022-JP?B?GyRCJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=40
string(236) "From: =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?GyRCTnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?GyRCJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?GyRCJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=41
string(236) "From: =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?GyRCTnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?GyRCJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?GyRCJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=42
string(236) "From: =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?GyRCTnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?GyRCJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?GyRCJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=43
string(212) "From: =?ISO-2022-JP?B?GyRCJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?GyRCO3pOc0Z8S1w4bBsoQg==?=
 =?ISO-2022-JP?B?GyRCJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=44
string(212) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?GyRCSjg7ek5zJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYlLRsoQg==?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=45
string(212) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?GyRCSjg7ek5zJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYlLRsoQg==?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=46
string(212) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?GyRCSjg7ek5zJTUlcxsoQg==?=
 =?ISO-2022-JP?B?GyRCJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYlLRsoQg==?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=47
string(176) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?GyRCSjg7ek5zJTUlcyVXJWsbKEI=?=
 =?ISO-2022-JP?B?GyRCSjg7ek5zRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?GyRCJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=48
string(180) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?GyRCO3pOcyU1JXMlVyVrSjgbKEI=?=
 =?ISO-2022-JP?B?GyRCO3pOc0Z8S1w4bCVGJS0bKEI=?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=49
string(180) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?GyRCO3pOcyU1JXMlVyVrSjgbKEI=?=
 =?ISO-2022-JP?B?GyRCO3pOc0Z8S1w4bCVGJS0bKEI=?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=50
string(180) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?GyRCO3pOcyU1JXMlVyVrSjgbKEI=?=
 =?ISO-2022-JP?B?GyRCO3pOc0Z8S1w4bCVGJS0bKEI=?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=51
string(152) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKOBsoQg==?=
 =?ISO-2022-JP?B?GyRCO3pOcyU1JXMlVyVrSjg7ehsoQg==?=
 =?ISO-2022-JP?B?GyRCTnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=52
string(148) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnNGfBsoQg==?=
 =?ISO-2022-JP?B?GyRCS1w4bCVGJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=53
string(148) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnNGfBsoQg==?=
 =?ISO-2022-JP?B?GyRCS1w4bCVGJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=54
string(148) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnNGfBsoQg==?=
 =?ISO-2022-JP?B?GyRCS1w4bCVGJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=55
string(148) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMbKEI=?=
 =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnNGfEtcOGwbKEI=?=
 =?ISO-2022-JP?B?GyRCJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=56
string(148) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNRsoQg==?=
 =?ISO-2022-JP?B?GyRCJXMlVyVrSjg7ek5zRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?GyRCJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=57
string(148) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNRsoQg==?=
 =?ISO-2022-JP?B?GyRCJXMlVyVrSjg7ek5zRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?GyRCJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=58
string(148) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNRsoQg==?=
 =?ISO-2022-JP?B?GyRCJXMlVyVrSjg7ek5zRnxLXDhsJUYbKEI=?=
 =?ISO-2022-JP?B?GyRCJS0lOSVIGyhC?="
-------- bool(true)
-------- line length=59
string(152) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNRsoQg==?=
 =?ISO-2022-JP?B?GyRCJXMlVyVrSjg7ek5zRnxLXDhsJUYlLRsoQg==?=
 =?ISO-2022-JP?B?GyRCJTklSBsoQg==?="
-------- bool(true)
-------- line length=60
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?GyRCJWtKODt6TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=61
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?GyRCJWtKODt6TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=62
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?GyRCJWtKODt6TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=63
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcbKEI=?=
 =?ISO-2022-JP?B?GyRCJWtKODt6TnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=64
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVclaxsoQg==?=
 =?ISO-2022-JP?B?GyRCSjg7ek5zRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=65
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVclaxsoQg==?=
 =?ISO-2022-JP?B?GyRCSjg7ek5zRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=66
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVclaxsoQg==?=
 =?ISO-2022-JP?B?GyRCSjg7ek5zRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=67
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVclaxsoQg==?=
 =?ISO-2022-JP?B?GyRCSjg7ek5zRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=68
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?GyRCTnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=69
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?GyRCTnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=70
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?GyRCTnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=71
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3obKEI=?=
 =?ISO-2022-JP?B?GyRCTnNGfEtcOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=72
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=73
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=74
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=75
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOcxsoQg==?=
 =?ISO-2022-JP?B?GyRCRnxLXDhsJUYlLSU5JUgbKEI=?="
-------- bool(true)
-------- line length=76
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOc0Z8S1wbKEI=?=
 =?ISO-2022-JP?B?GyRCOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=77
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOc0Z8S1wbKEI=?=
 =?ISO-2022-JP?B?GyRCOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=78
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOc0Z8S1wbKEI=?=
 =?ISO-2022-JP?B?GyRCOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
-------- line length=79
string(120) "From: =?ISO-2022-JP?B?GyRCJTUlcyVXJWtKODt6TnMlNSVzJVcla0o4O3pOc0Z8S1wbKEI=?=
 =?ISO-2022-JP?B?GyRCOGwlRiUtJTklSBsoQg==?="
-------- bool(true)
