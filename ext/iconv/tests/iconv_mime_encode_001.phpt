--TEST--
iconv_mime_encode(): Encoding parameter exceeds the maximum allowed for preference "input-charset"
--CREDITS--
Juan Carlos Morales (jcmargentina@gmail.com)
--EXTENSIONS--
iconv
--INI--
iconv.internal_charset=iso-8859-1
--FILE--
<?php
$extra = str_repeat("x", 256);

$preferences = [
    "input-charset" => "ISO-8859-1".$extra,
    "output-charset" => "UTF-8",
    "line-length" => 76,
    "line-break-chars" => "\n",
    "scheme" => "B"
];

$result = iconv_mime_encode("Subject", "Prüfung", $preferences);
var_dump($result);
?>
--EXPECTF--
Warning: iconv_mime_encode(): Encoding parameter exceeds the maximum allowed length of %d characters in %s on line %d
bool(false)