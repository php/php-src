--TEST--
Test mb_encode_mimeheader() function : basic functionality
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_encode_mimeheader') or die("skip mb_encode_mimeheader() is not available in this build");
?>
--FILE--
<?php
/*
 * Test mb_encode_header() with different strings
 */

echo "*** Testing mb_encode_mimeheader() : basic2 ***\n";

//All strings are the same when displayed in their respective encodings
$sjis_string = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
$jis_string = base64_decode('GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==');
$euc_jp_string = base64_decode('xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow==');

$inputs = array('SJIS' => $sjis_string,
                'JIS' => $jis_string,
                'EUC_JP' => $euc_jp_string);

foreach ($inputs as $lang => $input) {
    echo "\nLanguage: $lang\n";
    echo "-- Base 64: --\n";
    mb_internal_encoding($lang);
    $outEncoding = "UTF-8";
    var_dump(mb_encode_mimeheader($input, $outEncoding, 'B'));
    echo "-- Quoted-Printable --\n";
    var_dump(mb_encode_mimeheader($input, $outEncoding, 'Q'));
}

echo "Done";
?>
--EXPECT--
*** Testing mb_encode_mimeheader() : basic2 ***

Language: SJIS
-- Base 64: --
string(99) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJc=?=
 =?UTF-8?B?77yY77yZ44CC?="
-- Quoted-Printable --
string(201) "=?UTF-8?Q?=E6=97=A5=E6=9C=AC=E8=AA=9E=E3=83=86=E3=82=AD=E3=82=B9=E3=83=88?=
 =?UTF-8?Q?=E3=81=A7=E3=81=99=E3=80=82=30=31=32=33=34=EF=BC=95=EF=BC=96?=
 =?UTF-8?Q?=EF=BC=97=EF=BC=98=EF=BC=99=E3=80=82?="

Language: JIS
-- Base 64: --
string(99) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJc=?=
 =?UTF-8?B?77yY77yZ44CC?="
-- Quoted-Printable --
string(201) "=?UTF-8?Q?=E6=97=A5=E6=9C=AC=E8=AA=9E=E3=83=86=E3=82=AD=E3=82=B9=E3=83=88?=
 =?UTF-8?Q?=E3=81=A7=E3=81=99=E3=80=82=30=31=32=33=34=EF=BC=95=EF=BC=96?=
 =?UTF-8?Q?=EF=BC=97=EF=BC=98=EF=BC=99=E3=80=82?="

Language: EUC_JP
-- Base 64: --
string(99) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJc=?=
 =?UTF-8?B?77yY77yZ44CC?="
-- Quoted-Printable --
string(201) "=?UTF-8?Q?=E6=97=A5=E6=9C=AC=E8=AA=9E=E3=83=86=E3=82=AD=E3=82=B9=E3=83=88?=
 =?UTF-8?Q?=E3=81=A7=E3=81=99=E3=80=82=30=31=32=33=34=EF=BC=95=EF=BC=96?=
 =?UTF-8?Q?=EF=BC=97=EF=BC=98=EF=BC=99=E3=80=82?="
Done
