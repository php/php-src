--TEST--
bug #53946 (json_encode() with JSON_UNESCAPED_UNICODE)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
var_dump(json_encode("latin 1234 -/    russian мама мыла раму  specialchars \x02   \x08 \n   U+1D11E >𝄞<"));
var_dump(json_encode("latin 1234 -/    russian мама мыла раму  specialchars \x02   \x08 \n   U+1D11E >𝄞<", JSON_UNESCAPED_UNICODE));
var_dump(json_encode("ab\xE0"));
var_dump(json_encode("ab\xE0", JSON_UNESCAPED_UNICODE));
?>
--EXPECT--
string(156) ""latin 1234 -\/    russian \u043c\u0430\u043c\u0430 \u043c\u044b\u043b\u0430 \u0440\u0430\u043c\u0443  specialchars \u0002   \b \n   U+1D11E >\ud834\udd1e<""
string(100) ""latin 1234 -\/    russian мама мыла раму  specialchars \u0002   \b \n   U+1D11E >𝄞<""
bool(false)
bool(false)
