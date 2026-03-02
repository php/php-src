--TEST--
Temporary test of mbstring's HTML-ENTITIES 'encoding'
--EXTENSIONS--
mbstring
--FILE--
<?php

/* Using mbstring to convert strings to and from HTML entities has already been deprecated
 * So this test should be removed when the HTML-ENTITIES 'encoding' is */

function convertToEntities($raw, $htmlent) {
  $converted = mb_convert_encoding($raw, 'HTML-ENTITIES', 'UTF-8');
  if ($converted !== $htmlent)
    die('Expected ' . bin2hex($raw) . ' to convert to "' . $htmlent . '"; actually got "' . $converted . '"');
}

function convertFromEntities($raw, $htmlent) {
  $converted = mb_convert_encoding($htmlent, 'UTF-8', 'HTML-ENTITIES');
  if ($converted !== $raw)
    die('Expected "' . $htmlent . '" to convert to ' . bin2hex($raw) . '; actually got ' . bin2hex($converted));
}

function testConversion($raw, $htmlent) {
  convertToEntities($raw, $htmlent);
  convertFromEntities($raw, $htmlent);
}

testConversion('', '');
testConversion('abc', 'abc');
testConversion('&<>', '&<>');

convertFromEntities('"', '&quot;');
convertFromEntities("\xC3\xAC", '&igrave;');

convertFromEntities('あ', '&#x3042;');
testConversion('あ', '&#12354;');
testConversion('abcあxyz', 'abc&#12354;xyz');

convertFromEntities('&#x;', '&#x;');
convertFromEntities('&#;', '&#;');
convertFromEntities('&#', '&#');
convertFromEntities('&', '&');

convertFromEntities("\x00", '&#00000;');

testConversion(str_repeat('あ', 100), str_repeat('&#12354;', 100));

convertFromEntities("&;", "&;");
convertFromEntities("&f;", "&f;");

convertFromEntities("&A", "&&#65;");
convertFromEntities("&A", "&&#x41;");

echo "Done!\n";
?>
--EXPECTF--
Deprecated: mb_convert_encoding(): Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead in %s

Deprecated: mb_convert_encoding(): Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead in %s

Deprecated: mb_convert_encoding(): Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead in %s

Deprecated: mb_convert_encoding(): Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead in %s

Deprecated: mb_convert_encoding(): Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead in %s

Deprecated: mb_convert_encoding(): Handling HTML entities via mbstring is deprecated; use htmlspecialchars, htmlentities, or mb_encode_numericentity/mb_decode_numericentity instead in %s
Done!
