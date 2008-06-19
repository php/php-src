--TEST--
Bug #27303 (OCIBindByName binds numeric PHP values as characters)
--SKIPIF--
<?php
if (!extension_loaded('oci8')) die ("skip no oci8 extension");
require(dirname(__FILE__)."/connect.inc");
$sv = oci_server_version($c);
$sv = preg_match('/11.1/', $sv, $matches);
if ($sv !== 1) {
	die ("skip expected output only valid when using Oracle 11g database");
}
?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';
	
$create_st = array();
$create_st[] = "drop sequence myseq";
$create_st[] = "drop table mytab";
$create_st[] = "create sequence myseq";
$create_st[] = "create table mytab (mydata varchar2(20), seqcol number)";

foreach ($create_st as $statement) {
	$stmt = oci_parse($c, $statement);
	oci_execute($stmt);
}

define('MYLIMIT', 200);
define('INITMYBV', 11);

$stmt = "insert into mytab (mydata, seqcol) values ('Some data', myseq.nextval) returning seqcol into :mybv";

$stid = OCIParse($c, $stmt);
if (!$stid) { echo "Parse error"; die; }

//$mybv = INITMYBV;   // Uncomment this for the 2nd test only
$r = OCIBindByName($stid, ':MYBV', $mybv, 0 );  // Uncomment this for the 3rd test only
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

foreach ($create_st as $statement) {
	$stmt = oci_parse($c, $statement);
	oci_execute($stmt);
}

echo "Done\n";
?>
--EXPECTF--	
unicode(1) "1"
unicode(1) "2"
unicode(1) "3"
unicode(1) "4"
unicode(1) "5"
unicode(1) "6"
unicode(1) "7"
unicode(1) "8"
unicode(1) "9"
unicode(2) "10"
unicode(2) "11"
unicode(2) "12"
unicode(2) "13"
unicode(2) "14"
unicode(2) "15"
unicode(2) "16"
unicode(2) "17"
unicode(2) "18"
unicode(2) "19"
unicode(2) "20"
unicode(2) "21"
unicode(2) "22"
unicode(2) "23"
unicode(2) "24"
unicode(2) "25"
unicode(2) "26"
unicode(2) "27"
unicode(2) "28"
unicode(2) "29"
unicode(2) "30"
unicode(2) "31"
unicode(2) "32"
unicode(2) "33"
unicode(2) "34"
unicode(2) "35"
unicode(2) "36"
unicode(2) "37"
unicode(2) "38"
unicode(2) "39"
unicode(2) "40"
unicode(2) "41"
unicode(2) "42"
unicode(2) "43"
unicode(2) "44"
unicode(2) "45"
unicode(2) "46"
unicode(2) "47"
unicode(2) "48"
unicode(2) "49"
unicode(2) "50"
unicode(2) "51"
unicode(2) "52"
unicode(2) "53"
unicode(2) "54"
unicode(2) "55"
unicode(2) "56"
unicode(2) "57"
unicode(2) "58"
unicode(2) "59"
unicode(2) "60"
unicode(2) "61"
unicode(2) "62"
unicode(2) "63"
unicode(2) "64"
unicode(2) "65"
unicode(2) "66"
unicode(2) "67"
unicode(2) "68"
unicode(2) "69"
unicode(2) "70"
unicode(2) "71"
unicode(2) "72"
unicode(2) "73"
unicode(2) "74"
unicode(2) "75"
unicode(2) "76"
unicode(2) "77"
unicode(2) "78"
unicode(2) "79"
unicode(2) "80"
unicode(2) "81"
unicode(2) "82"
unicode(2) "83"
unicode(2) "84"
unicode(2) "85"
unicode(2) "86"
unicode(2) "87"
unicode(2) "88"
unicode(2) "89"
unicode(2) "90"
unicode(2) "91"
unicode(2) "92"
unicode(2) "93"
unicode(2) "94"
unicode(2) "95"
unicode(2) "96"
unicode(2) "97"
unicode(2) "98"
unicode(2) "99"
unicode(3) "100"
unicode(3) "101"
unicode(3) "102"
unicode(3) "103"
unicode(3) "104"
unicode(3) "105"
unicode(3) "106"
unicode(3) "107"
unicode(3) "108"
unicode(3) "109"
unicode(3) "110"
unicode(3) "111"
unicode(3) "112"
unicode(3) "113"
unicode(3) "114"
unicode(3) "115"
unicode(3) "116"
unicode(3) "117"
unicode(3) "118"
unicode(3) "119"
unicode(3) "120"
unicode(3) "121"
unicode(3) "122"
unicode(3) "123"
unicode(3) "124"
unicode(3) "125"
unicode(3) "126"
unicode(3) "127"
unicode(3) "128"
unicode(3) "129"
unicode(3) "130"
unicode(3) "131"
unicode(3) "132"
unicode(3) "133"
unicode(3) "134"
unicode(3) "135"
unicode(3) "136"
unicode(3) "137"
unicode(3) "138"
unicode(3) "139"
unicode(3) "140"
unicode(3) "141"
unicode(3) "142"
unicode(3) "143"
unicode(3) "144"
unicode(3) "145"
unicode(3) "146"
unicode(3) "147"
unicode(3) "148"
unicode(3) "149"
unicode(3) "150"
unicode(3) "151"
unicode(3) "152"
unicode(3) "153"
unicode(3) "154"
unicode(3) "155"
unicode(3) "156"
unicode(3) "157"
unicode(3) "158"
unicode(3) "159"
unicode(3) "160"
unicode(3) "161"
unicode(3) "162"
unicode(3) "163"
unicode(3) "164"
unicode(3) "165"
unicode(3) "166"
unicode(3) "167"
unicode(3) "168"
unicode(3) "169"
unicode(3) "170"
unicode(3) "171"
unicode(3) "172"
unicode(3) "173"
unicode(3) "174"
unicode(3) "175"
unicode(3) "176"
unicode(3) "177"
unicode(3) "178"
unicode(3) "179"
unicode(3) "180"
unicode(3) "181"
unicode(3) "182"
unicode(3) "183"
unicode(3) "184"
unicode(3) "185"
unicode(3) "186"
unicode(3) "187"
unicode(3) "188"
unicode(3) "189"
unicode(3) "190"
unicode(3) "191"
unicode(3) "192"
unicode(3) "193"
unicode(3) "194"
unicode(3) "195"
unicode(3) "196"
unicode(3) "197"
unicode(3) "198"
unicode(3) "199"
Done
