--TEST--
Test iconv() function : basic functionality
--EXTENSIONS--
iconv
--FILE--
<?php
/*
 * Test basic functionality of iconv()
 */

echo "*** Testing iconv() : basic functionality ***\n";

//All strings are the same when displayed in their respective encodings
$sjis_string = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
$euc_jp_string = base64_decode('xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow==');
$utf8_string = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

echo "\n-- Convert to EUC-JP --\n";
echo "Expected EUC-JP encoded string in base64:\n";
var_dump(bin2hex($euc_jp_string));
echo "Converted Strings:\n";
var_dump(bin2hex(iconv('SJIS', 'EUC-JP', $sjis_string )));
var_dump(bin2hex(iconv('UTF-8', 'EUC-JP', $utf8_string)));

echo "\n-- Convert to SJIS --\n";
echo "Expected SJIS encoded string in base64:\n";
var_dump(bin2hex($sjis_string));
echo "Converted Strings:\n";
var_dump(bin2hex(iconv('EUC-JP', 'SJIS', $euc_jp_string)));
var_dump(bin2hex(iconv('UTF-8', 'SJIS', $utf8_string)));

echo "\n-- Convert to UTF-8 --\n";
echo "Expected UTF-8 encoded string in base64:\n";
var_dump(bin2hex($utf8_string));
echo "Converted Strings:\n";
var_dump(bin2hex(iconv('SJIS', 'UTF-8', $sjis_string)));
var_dump(bin2hex(iconv('EUC-JP', 'UTF-8', $euc_jp_string)));

echo "Done";
?>
--EXPECT--
*** Testing iconv() : basic functionality ***

-- Convert to EUC-JP --
Expected EUC-JP encoded string in base64:
string(74) "c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3"
Converted Strings:
string(74) "c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3"
string(74) "c6fccbdcb8eca5c6a5ada5b9a5c8a4c7a4b9a1a33031323334a3b5a3b6a3b7a3b8a3b9a1a3"

-- Convert to SJIS --
Expected SJIS encoded string in base64:
string(74) "93fa967b8cea8365834c8358836782c582b781423031323334825482558256825782588142"
Converted Strings:
string(74) "93fa967b8cea8365834c8358836782c582b781423031323334825482558256825782588142"
string(74) "93fa967b8cea8365834c8358836782c582b781423031323334825482558256825782588142"

-- Convert to UTF-8 --
Expected UTF-8 encoded string in base64:
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
Converted Strings:
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
string(106) "e697a5e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
Done
