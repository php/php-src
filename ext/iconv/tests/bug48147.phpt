--TEST--
Bug #48147 (iconv with //IGNORE cuts the string)
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php
$text = "aa\xC3\xC3\xC3\xB8aa";
var_dump(iconv("UTF-8", "UTF-8", $text));
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", $text)));
// only invalid
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", "\xC3")));
// start invalid
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", "\xC3\xC3\xC3\xB8aa")));
// finish invalid
var_dump(urlencode(iconv("UTF-8", "UTF-8//IGNORE", "aa\xC3\xC3\xC3")));
?>
--EXPECTF--
Notice: iconv(): Detected an illegal character in input string in %s on line %d
bool(false)
string(10) "aa%C3%B8aa"

Notice: iconv(): Detected an incomplete multibyte character in input string in %s on line %d
string(0) ""
string(8) "%C3%B8aa"

Notice: iconv(): Detected an incomplete multibyte character in input string in %s on line %d
string(0) ""
