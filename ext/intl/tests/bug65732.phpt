--TEST--
Bug #65732 (grapheme_*() is not Unicode compliant on CR LF sequence)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not available');
?>
--FILE--
<?php
var_dump(grapheme_strlen("\r\n"));
var_dump(grapheme_substr(implode("\r\n", ['abc', 'def', 'ghi']), 5));
var_dump(grapheme_strrpos("a\r\nb", 'b'));
?>
==DONE==
--EXPECT--
int(1)
string(7) "ef
ghi"
int(2)
==DONE==
