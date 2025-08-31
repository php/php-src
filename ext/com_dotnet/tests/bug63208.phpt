--TEST--
Bug #63208 (BSTR to PHP string conversion not binary safe)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
$string = "\u{0905}b\0cd";
$variant = new VARIANT($string, VT_ARRAY | VT_UI1, CP_UTF8); // Array of bytes
$converted = (string) $variant;
var_dump(bin2hex($string));
var_dump(bin2hex($converted));
?>
--EXPECT--
string(14) "e0a48562006364"
string(14) "e0a48562006364"
