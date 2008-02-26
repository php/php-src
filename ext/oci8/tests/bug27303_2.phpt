--TEST--
Bug #27303 (OCIBindByName binds numeric PHP values as characters)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
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

$mybv = INITMYBV;   // Uncomment this for the 2nd test only
$r = OCIBindByName($stid, ':MYBV', $mybv /*, 5 */);  // Uncomment this for the 3rd test only
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
string(2) "10"
string(2) "10"
string(2) "10"
string(2) "10"
string(2) "10"
string(2) "10"
string(2) "10"
string(2) "10"
string(2) "10"
string(2) "10"
string(2) "11"
string(2) "11"
string(2) "11"
string(2) "11"
string(2) "11"
string(2) "11"
string(2) "11"
string(2) "11"
string(2) "11"
string(2) "11"
string(2) "12"
string(2) "12"
string(2) "12"
string(2) "12"
string(2) "12"
string(2) "12"
string(2) "12"
string(2) "12"
string(2) "12"
string(2) "12"
string(2) "13"
string(2) "13"
string(2) "13"
string(2) "13"
string(2) "13"
string(2) "13"
string(2) "13"
string(2) "13"
string(2) "13"
string(2) "13"
string(2) "14"
string(2) "14"
string(2) "14"
string(2) "14"
string(2) "14"
string(2) "14"
string(2) "14"
string(2) "14"
string(2) "14"
string(2) "14"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "15"
string(2) "16"
string(2) "16"
string(2) "16"
string(2) "16"
string(2) "16"
string(2) "16"
string(2) "16"
string(2) "16"
string(2) "16"
string(2) "16"
string(2) "17"
string(2) "17"
string(2) "17"
string(2) "17"
string(2) "17"
string(2) "17"
string(2) "17"
string(2) "17"
string(2) "17"
string(2) "17"
string(2) "18"
string(2) "18"
string(2) "18"
string(2) "18"
string(2) "18"
string(2) "18"
string(2) "18"
string(2) "18"
string(2) "18"
string(2) "18"
string(2) "19"
string(2) "19"
string(2) "19"
string(2) "19"
string(2) "19"
string(2) "19"
string(2) "19"
string(2) "19"
string(2) "19"
string(2) "19"
Done
