--TEST--
Test mime_content_type() function : variation 
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

echo "*** Testing mime_content_type() : variation ***\n";


var_dump(mime_content_type("mime_test_wrong.jpeg"));

// try a zero length file
$tmpFile = __FILE__.'tmp';
$f = fopen($tmpFile,"w");
fclose($f);

var_dump(mime_content_type($tmpFile));
unlink($tmpFile);

?>
===DONE===
--EXPECT--
*** Testing mime_content_type() : variation ***
bool(false)
string(10) "text/plain"
===DONE===
