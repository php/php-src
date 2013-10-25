--TEST--
Bug #27303 (OCIBindByName binds numeric PHP values as characters)
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
require(dirname(__FILE__)."/connect.inc");
// The bind buffer size edge cases seem to change each DB version.
preg_match('/.*Release ([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)\.([[:digit:]]+)*/', oci_server_version($c), $matches);
if (!(isset($matches[0]) && $matches[1] >= 12)) {
    die("skip expected output only valid when using Oracle 12c database");
}
?>
--ENV--
NLS_LANG=
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';
	
$create_st = array();
$create_st[] = "drop sequence myseq";
$create_st[] = "drop table mytab";
$create_st[] = "create sequence myseq";
$create_st[] = "create table mytab (mydata varchar2(20), seqcol number)";

oci8_test_sql_execute($c, $create_st);

define('MYLIMIT', 200);

$stmt = "insert into mytab (mydata, seqcol) values ('Some data', myseq.nextval) returning seqcol into :mybv";

$stid = OCIParse($c, $stmt);
if (!$stid) { echo "Parse error"; die; }

$r = OCIBindByName($stid, ':MYBV', $mybv, 0 );
if (!$r) { echo "Bind error"; die; }

for ($i = 1; $i < MYLIMIT; $i++) {
	$r = OCIExecute($stid, OCI_DEFAULT);
	if (!$r) { echo "Execute error"; die; }
	var_dump($mybv);
}

OCICommit($c);

$drop_st = array();
$drop_st[] = "drop sequence myseq";
$drop_st[] = "drop table mytab";

oci8_test_sql_execute($c, $drop_st);

echo "Done\n";
?>
--EXPECT--	
string(1) "1"
string(1) "2"
string(1) "3"
string(1) "4"
string(1) "5"
string(1) "6"
string(1) "7"
string(1) "8"
string(1) "9"
string(2) "10"
string(2) "11"
string(2) "12"
string(2) "13"
string(2) "14"
string(2) "15"
string(2) "16"
string(2) "17"
string(2) "18"
string(2) "19"
string(2) "20"
string(2) "21"
string(2) "22"
string(2) "23"
string(2) "24"
string(2) "25"
string(2) "26"
string(2) "27"
string(2) "28"
string(2) "29"
string(2) "30"
string(2) "31"
string(2) "32"
string(2) "33"
string(2) "34"
string(2) "35"
string(2) "36"
string(2) "37"
string(2) "38"
string(2) "39"
string(2) "40"
string(2) "41"
string(2) "42"
string(2) "43"
string(2) "44"
string(2) "45"
string(2) "46"
string(2) "47"
string(2) "48"
string(2) "49"
string(2) "50"
string(2) "51"
string(2) "52"
string(2) "53"
string(2) "54"
string(2) "55"
string(2) "56"
string(2) "57"
string(2) "58"
string(2) "59"
string(2) "60"
string(2) "61"
string(2) "62"
string(2) "63"
string(2) "64"
string(2) "65"
string(2) "66"
string(2) "67"
string(2) "68"
string(2) "69"
string(2) "70"
string(2) "71"
string(2) "72"
string(2) "73"
string(2) "74"
string(2) "75"
string(2) "76"
string(2) "77"
string(2) "78"
string(2) "79"
string(2) "80"
string(2) "81"
string(2) "82"
string(2) "83"
string(2) "84"
string(2) "85"
string(2) "86"
string(2) "87"
string(2) "88"
string(2) "89"
string(2) "90"
string(2) "91"
string(2) "92"
string(2) "93"
string(2) "94"
string(2) "95"
string(2) "96"
string(2) "97"
string(2) "98"
string(2) "99"
string(3) "100"
string(3) "101"
string(3) "102"
string(3) "103"
string(3) "104"
string(3) "105"
string(3) "106"
string(3) "107"
string(3) "108"
string(3) "109"
string(3) "110"
string(3) "111"
string(3) "112"
string(3) "113"
string(3) "114"
string(3) "115"
string(3) "116"
string(3) "117"
string(3) "118"
string(3) "119"
string(3) "120"
string(3) "121"
string(3) "122"
string(3) "123"
string(3) "124"
string(3) "125"
string(3) "126"
string(3) "127"
string(3) "128"
string(3) "129"
string(3) "130"
string(3) "131"
string(3) "132"
string(3) "133"
string(3) "134"
string(3) "135"
string(3) "136"
string(3) "137"
string(3) "138"
string(3) "139"
string(3) "140"
string(3) "141"
string(3) "142"
string(3) "143"
string(3) "144"
string(3) "145"
string(3) "146"
string(3) "147"
string(3) "148"
string(3) "149"
string(3) "150"
string(3) "151"
string(3) "152"
string(3) "153"
string(3) "154"
string(3) "155"
string(3) "156"
string(3) "157"
string(3) "158"
string(3) "159"
string(3) "160"
string(3) "161"
string(3) "162"
string(3) "163"
string(3) "164"
string(3) "165"
string(3) "166"
string(3) "167"
string(3) "168"
string(3) "169"
string(3) "170"
string(3) "171"
string(3) "172"
string(3) "173"
string(3) "174"
string(3) "175"
string(3) "176"
string(3) "177"
string(3) "178"
string(3) "179"
string(3) "180"
string(3) "181"
string(3) "182"
string(3) "183"
string(3) "184"
string(3) "185"
string(3) "186"
string(3) "187"
string(3) "188"
string(3) "189"
string(3) "190"
string(3) "191"
string(3) "192"
string(3) "193"
string(3) "194"
string(3) "195"
string(3) "196"
string(3) "197"
string(3) "198"
string(3) "199"
Done
