--TEST--
GH-21664 (iconv_mime_decode/iconv_mime_encode bailout corruption)
--EXTENSIONS--
iconv
--FILE--
<?php
$r = iconv_mime_decode("=?utf-8?Q?" . chr(0xA1) . "?= .");
var_dump($r);

$r = iconv_mime_encode("Subject", "\x80", ["input-charset" => "UTF-8", "output-charset" => "UTF-8"]);
var_dump($r);

echo "Done\n";
?>
--EXPECTF--
Notice: iconv_mime_decode(): Detected an illegal character in input string in %s on line %d
bool(false)

Notice: iconv_mime_encode(): Detected an illegal character in input string in %s on line %d
bool(false)
Done
