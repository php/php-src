--TEST--
Bug #72330 (CSV fields incorrectly split if escape char followed by UTF chars)
--SKIPIF--
<?php
if (setlocale(LC_ALL, "en_US.utf8", "en_AU.utf8", "ko_KR.utf8", "zh_CN.utf8", "de_DE.utf8", "es_EC.utf8", "fr_FR.utf8", "ja_JP.utf8", "el_GR.utf8", "nl_NL.utf8") === false) {
    die('skip available locales not usable');
}
?>
--FILE--
<?php
setlocale(LC_ALL, "en_US.utf8", "en_AU.utf8", "ko_KR.utf8", "zh_CN.utf8", "de_DE.utf8", "es_EC.utf8", "fr_FR.utf8", "ja_JP.utf8", "el_GR.utf8", "nl_NL.utf8");

$utf_1 = chr(0xD1) . chr(0x81); // U+0440;
$utf_2   = chr(0xD8) . chr(0x80); // U+0600

$string = '"first #' . $utf_1 . $utf_2 . '";"second"';
$fields = str_getcsv($string, ';', '"', "#");
var_dump($fields);
?>
--EXPECT--
array(2) {
  [0]=>
  string(11) "first #с؀"
  [1]=>
  string(6) "second"
}
