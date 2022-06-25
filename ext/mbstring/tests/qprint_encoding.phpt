--TEST--
Temporary test of mbstring's QPrint 'encoding'
--EXTENSIONS--
mbstring
--FILE--
<?php

/* Using mbstring to convert strings to and from QPrint has already been deprecated
 * So this test should be removed when the QPrint 'encoding' is */

function testConversion($raw, $qprint) {
  $converted = mb_convert_encoding($raw, 'QPrint', '8bit');
  if ($converted !== $qprint)
    die('Expected ' . bin2hex($raw) . ' to convert to "' . $qprint . '"; actually got "' . $converted . '"');
  $converted = mb_convert_encoding($qprint, '8bit', 'QPrint');
  if ($converted !== str_replace("\n", "\r\n", $raw))
    die('Expected "' . $qprint . '" to convert to ' . bin2hex($raw) . '; actually got ' . bin2hex($converted));
}

testConversion('', '');
testConversion('あ', '=E3=81=82');

testConversion("J'interdis aux marchands de vanter trop leurs marchandises. Car ils se font vite pédagogues et t'enseignent comme but ce qui n'est par essence qu'un moyen, et te trompant ainsi sur la route à suivre les voilà bientôt qui te dégradent, car si leur musique est vulgaire ils te fabriquent pour te la vendre une âme vulgaire.
—\xE2\x80\x89Antoine de Saint-Exupéry, Citadelle", "J'interdis aux marchands de vanter trop leurs marchandises. Car ils se f=\r\nont vite p=C3=A9dagogues et t'enseignent comme but ce qui n'est par esse=\r\nnce qu'un moyen, et te trompant ainsi sur la route =C3=A0 suivre les voi=\r\nl=C3=A0 bient=C3=B4t qui te d=C3=A9gradent, car si leur musique est vulg=\r\naire ils te fabriquent pour te la vendre une =C3=A2me vulgaire.\r\n=E2=80=94=E2=80=89Antoine de Saint-Exup=C3=A9ry, Citadelle");

// Regression test, found by a fuzzer
testConversion("***,*-S,\xac,,\xb7,l,,,,\xb6UTF7,\xb5\xb5\xb5\xb5\xb5\xb5K\xb5\xb5!\xa4\xa4\xa4\xa4\xa4\xbd\xb5,\xb5\xb5\xb5\xb5\xb5\xb5\xb5\xb5\xb5&", "***,*-S,=AC,,=B7,l,,,,=B6UTF7,=B5=B5=B5=B5=B5=B5K=B5=B5!=A4=A4=A4=A4=A4=BD=\r\n=B5,=B5=B5=B5=B5=B5=B5=B5=B5=B5&");

echo "Done!\n";
?>
--EXPECTF--
Deprecated: mb_convert_encoding(): Handling QPrint via mbstring is deprecated; use quoted_printable_encode/quoted_printable_decode instead in %s

Deprecated: mb_convert_encoding(): Handling QPrint via mbstring is deprecated; use quoted_printable_encode/quoted_printable_decode instead in %s

Deprecated: mb_convert_encoding(): Handling QPrint via mbstring is deprecated; use quoted_printable_encode/quoted_printable_decode instead in %s

Deprecated: mb_convert_encoding(): Handling QPrint via mbstring is deprecated; use quoted_printable_encode/quoted_printable_decode instead in %s
Done!
