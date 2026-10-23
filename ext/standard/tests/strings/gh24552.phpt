--TEST--
GH-24552 (Case-insensitive substr_compare() is locale-dependent)
--SKIPIF--
<?php
if (setlocale(LC_CTYPE, 'de_DE.ISO8859-1', 'German_Germany.1252') === false) {
    die('skip de_DE.ISO8859-1 or German_Germany.1252 locale not available');
}
?>
--FILE--
<?php
setlocale(LC_CTYPE, 'de_DE.ISO8859-1', 'German_Germany.1252');
// Case folding is ASCII-only, so "\xC4" (Ä) and "\xE4" (ä) differ.
var_dump(substr_compare("\xC4", "\xE4", 0, 1, true));
?>
--EXPECT--
int(-32)
