--TEST--
Bug #68447: grapheme_extract take an extra trailing character
--EXTENSIONS--
intl
--FILE--
<?php
$katsushikaku = "葛󠄁飾区";
echo grapheme_extract($katsushikaku, 1) . "\n";

$haiyore = "這󠄀いよれ";
echo grapheme_extract($haiyore, 1, GRAPHEME_EXTR_COUNT) . "\n";
echo grapheme_extract($haiyore, 2, GRAPHEME_EXTR_COUNT) . "\n";
echo grapheme_extract($haiyore, 6, GRAPHEME_EXTR_MAXBYTES) . "\n";
echo grapheme_extract($haiyore, 9, GRAPHEME_EXTR_MAXBYTES) . "\n";
echo grapheme_extract($haiyore, 12, GRAPHEME_EXTR_MAXBYTES) . "\n";
echo grapheme_extract($haiyore, 1, GRAPHEME_EXTR_MAXCHARS) . "\n";
echo grapheme_extract($haiyore, 2, GRAPHEME_EXTR_MAXCHARS) . "\n";
echo grapheme_extract($haiyore, 3, GRAPHEME_EXTR_MAXCHARS) . "\n";
?>
--EXPECT--
葛󠄁
這󠄀
這󠄀い

這󠄀
這󠄀い

這󠄀
這󠄀い
