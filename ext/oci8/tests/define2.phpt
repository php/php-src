--TEST--
Test oci_define_by_name types
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__)."/connect.inc";

$stmt = oci_parse($c, "create table phptestrawtable( id number(10), fileimage raw(1000))");
oci_execute($stmt);

$stmt = oci_parse ($c, "insert into phptestrawtable (id, fileimage) values (:id, :fileimage)");
$i=1;
$fileimage = file_get_contents( dirname(__FILE__)."/test.gif");
$fileimage = substr($fileimage, 0, 300);
var_dump(md5($fileimage));

oci_bind_by_name( $stmt, ":id", $i, -1);
oci_bind_by_name( $stmt, ":fileimage", $fileimage, -1, SQLT_BIN);
oci_execute($stmt, OCI_DEFAULT);
oci_commit($c);

echo "Test 1\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phptestrawtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $fi));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($fi);
	echo "file md5:" . md5((binary)$fi) . "\n";
}

echo "Test 2\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phptestrawtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $fi));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($fi);
	echo "file md5:" . md5((binary)$fi) . "\n";
}

echo "Test 3 - test repeatability\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phptestrawtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $fi, SQLT_STR));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($fi);
	echo "file md5:" . md5((binary)$fi) . "\n";
}

echo "Test 4 - wrong type\n";
$stmt = oci_parse($c, "SELECT fileimage FROM phptestrawtable");
var_dump(oci_define_by_name($stmt, 'FILEIMAGE', $fi, SQLT_RSET));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($fi);
	echo "file md5:" . md5((binary)$fi) . "\n";
}

$stmt = oci_parse($c, "drop table phptestrawtable");
oci_execute($stmt);

echo "Done\n";
?>
--EXPECTF--
unicode(32) "88b274d7a257ac6f70435b83abd4e26e"
Test 1
bool(true)
unicode(300) "GIF89%s"
file md5:88b274d7a257ac6f70435b83abd4e26e
Test 2
bool(true)
unicode(300) "GIF89%s"
file md5:88b274d7a257ac6f70435b83abd4e26e
Test 3 - test repeatability
bool(true)
unicode(1200) "4700490046003800390061007800000043000000E6006A0000007F008200B800390037004700280025002A00CC00CD00E200A100A400CB00D300D500E700B200B400D40043004200580083008600B90082008300B30052005200720090009200C200C200C400DE00AA00AC00D0004C004B0063005B005C008300DD00DE00EC003B0038003C006E007100A5006A006D009D00610063008D0075007900B1007B007E00B500E500E600F00099009C00C6008C008D00C100B900BA00D9006B006B0092004E004E006B0071007400A9007A007A00A30088008B00BD0072007400A3007400730098008E009000C1005A005B007E00E200E300EF007B007D00AD00A400A500D0006D007000A20072007600AC0095009600C800BB00BD00D90074007800AE0085008800BB0092009500C300D800D900EA0092009200C4006400660092006B006E009F00A500A800CE0094009600C5002E002B002F00530051006800B300B400D7006C006A008C005C005B0076008A008D00BF0066006800960068006A009A009C009F00C80031002E003900AE00B000D3009C009C00CD005500560078009E00A100CA0096009900C50081008200AF006700690097003F003D005000BC00BE00DA005E006000890098009900C8008C008E00BF0089008A00BA00570058007C00B600B700D700D500D700E80022001E0020006C006F009E00CE00D000E400BF00C000DC0077007B00B40076007800A7005F005E007D0099009900CC006E006F00980073007700AE0022001E001F00FF00FF00FF0090008E008F00590056005700C700C600C700EE00EE00F500D500D400D500F600F600"
file md5:d60f46611ba231f0052a9ceca1346ff1
Test 4 - wrong type
bool(true)

Warning: oci_fetch(): ORA-00932: inconsistent datatypes%s in %s on line %d
Done
