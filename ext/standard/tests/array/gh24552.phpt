--TEST--
GH-24552 (ksort() and krsort() with SORT_FLAG_CASE are locale-dependent)
--SKIPIF--
<?php
if (setlocale(LC_CTYPE, 'de_DE.ISO8859-1', 'German_Germany.1252') === false) {
    die('skip de_DE.ISO8859-1 or German_Germany.1252 locale not available');
}
?>
--FILE--
<?php
setlocale(LC_CTYPE, 'de_DE.ISO8859-1', 'German_Germany.1252');

// Case folding is ASCII-only, so "\xC4" (Ä) sorts before "\xE0" (à).
$array = ["\xE0" => 0, "\xC4" => 0];
ksort($array, SORT_STRING | SORT_FLAG_CASE);
echo implode(' ', array_map(bin2hex(...), array_keys($array))), "\n";
krsort($array, SORT_STRING | SORT_FLAG_CASE);
echo implode(' ', array_map(bin2hex(...), array_keys($array))), "\n";
?>
--EXPECT--
c4 e0
e0 c4
