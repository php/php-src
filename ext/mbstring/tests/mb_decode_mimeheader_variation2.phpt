--TEST--
Test mb_decode_mimeheader() function : variation
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_decode_mimeheader') or die("skip mb_decode_mimeheader() is not available in this build");
?>
--FILE--
<?php
echo "*** Testing mb_decode_mimeheader() : variation ***\n";
mb_internal_encoding('utf-8');

//all the following are identical, we will convert to utf-8

$encoded_words = array(
"=?Shift_JIS?B?k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==?=",
"=?Shift_JIS?Q?=93=FA=96=7B=8C=EA=83e=83L=83X=83g=82=C5=82=B7=81B=30=31=32?=
=?Shift_JIS?Q?=33=34=82T=82U=82V=82W=82X=81B?=",

"=?ISO-2022-JP?B?GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYbKEI=?=
=?ISO-2022-JP?B?GyRCIzcjOCM5ISMbKEI=?=",
"=?ISO-2022-JP?Q?=1B=24BF=7CK=5C=38l=25F=25-=25=39=25H=24G=24=39=1B=28B?=
=?ISO-2022-JP?Q?=1B=24B!=23=1B=28B=30=31=32=33=34=1B=24B=23=35=1B=28B?=
=?ISO-2022-JP?Q?=1B=24B=23=36=23=37=23=38=23=39!=23=1B=28B?=",

"=?EUC-JP?B?xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow==?=",
"=?EUC-JP?Q?=C6=FC=CB=DC=B8=EC=A5=C6=A5=AD=A5=B9=A5=C8=A4=C7=A4=B9=A1=A3?=
=?EUC-JP?Q?=30=31=32=33=34=A3=B5=A3=B6=A3=B7=A3=B8=A3=B9=A1=A3?="
 );

 foreach ($encoded_words as $word) {
    var_dump(bin2hex(mb_decode_mimeheader($word)));
}

?>
--EXPECT--
*** Testing mb_decode_mimeheader() : variation ***
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
