--TEST--
Bug #79986 (str_ireplace bug with diacritics characters)
--SKIPIF--
<?php
if (!setlocale(LC_ALL, 'de_DE.ISO-8859-1', 'de-DE')) die('skip German locale not available');
?>
--FILE--
<?php
setlocale(LC_ALL, 'de_DE.ISO-8859-1', 'de-DE');
echo str_ireplace(["\xE4", "\xF6", "\xFC"], ['1', '2', '3'], "\xE4\xC4 \xF6\xD6 \xFC\xDC") . PHP_EOL;
?>
--EXPECT--
11 22 33
