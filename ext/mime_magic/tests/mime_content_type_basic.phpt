--TEST--
Test mime_content_type() function : basic test
--SKIPIF--
<?php if (!extension_loaded("mime_magic")) print "skip"; ?>
--INI--
mime_magic.magicfile = magic.mime
mime_magic.debug = 0
--FILE--
<?php
/* Prototype  : string mime_content_type(string filename|resource stream)
 * Description: Return content-type for file 
 * Source code: ext/mime_magic/mime_magic.c
 * Alias to functions: 
 */
chdir(dirname(__FILE__));
echo "--- testing files ---\n";
var_dump(mime_content_type("mime_test.jpeg"));
var_dump(mime_content_type("mime_test.png"));
var_dump(mime_content_type("mime_test.gif"));
var_dump(mime_content_type("mime_test.bmp"));
var_dump(mime_content_type("mime_test.wav"));
var_dump(mime_content_type("mime_test.zip"));
var_dump(mime_content_type("mime_test.html"));


// repeat using streams
echo "\n--- testing streams ---\n";
$res = fopen("mime_test.jpeg", "rb");
var_dump(mime_content_type($res));
fclose($res);
$res = fopen("mime_test.png", "rb");
var_dump(mime_content_type($res));
fclose($res);
$res = fopen("mime_test.gif", "rb");
var_dump(mime_content_type($res));
fclose($res);
$res = fopen("mime_test.bmp", "rb");
var_dump(mime_content_type($res));
fclose($res);
$res = fopen("mime_test.wav", "rb");
var_dump(mime_content_type($res));
fclose($res);
$res = fopen("mime_test.zip", "rb");
var_dump(mime_content_type($res));
fclose($res);
$res = fopen("mime_test.html", "rb");
var_dump(mime_content_type($res));
fclose($res);
echo "Done";
?>
--EXPECT--
--- testing files ---
string(10) "image/jpeg"
string(11) "image/x-png"
string(9) "image/gif"
string(11) "image/x-bmp"
string(11) "audio/x-wav"
string(15) "application/zip"
string(9) "text/html"

--- testing streams ---
string(10) "image/jpeg"
string(11) "image/x-png"
string(9) "image/gif"
string(11) "image/x-bmp"
string(11) "audio/x-wav"
string(15) "application/zip"
string(9) "text/html"
Done