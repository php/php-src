--TEST--
PDO_OCI: stream_get_contents length & offset test
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_oci')) die('skip not loaded');
require(dirname(__FILE__).'/../../pdo/tests/pdo_test.inc');
if (!strpos(strtolower(getenv('PDOTEST_DSN')), 'charset=we8mswin1252')) die('skip expected output valid for WE8MSWIN1252 character set');
PDOTest::skip();
?>
--FILE--
<?php

require(dirname(__FILE__) . '/../../pdo/tests/pdo_test.inc');

$dbh = PDOTest::factory();

$dbh->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

// Initialization
@$dbh->exec("drop table pdo_oci_stream_1_tab");

$stmtarray = array(
	"create table pdo_oci_stream_1_tab (id number, data clob)",
);

foreach ($stmtarray as $stmt) {
	$dbh->exec($stmt);
}

$dbh->exec("
  declare
    lob1 clob := 'abc' || lpad('j',30000,'j') || 'xyz';
   begin
    insert into pdo_oci_stream_1_tab (id,data) values (1, 'abcdefghijklmnopqrstuvwxyz');
    insert into pdo_oci_stream_1_tab (id,data) values (2, lob1);
  end;");

echo "Test 1\n";

$s = $dbh->prepare("select data from pdo_oci_stream_1_tab where id = 1");
$s->execute();
$r = $s->fetch();

// stream_get_contents ( resource $handle [, int $maxlength = -1 [, int $offset = -1 ]] )
echo 'Read '.stream_get_contents($r['data'], 1, 1)."$\n";  // b
echo 'Read '.stream_get_contents($r['data'], 2, 1)."$\n";  // cd
echo 'Read '.stream_get_contents($r['data'], 2, 0)."$\n";  // ab
echo 'Read '.stream_get_contents($r['data'], 26, 0)."$\n"; // abcdefghijklmnopqrstuvwxyz
echo 'Read '.stream_get_contents($r['data'], 27, 0)."$\n"; // abcdefghijklmnopqrstuvwxyz
echo 'Read '.stream_get_contents($r['data'], 27, 1)."$\n"; // bcdefghijklmnopqrstuvwxyz
echo 'Read '.stream_get_contents($r['data'], 1, 20)."$\n"; // u
echo 'Read '.stream_get_contents($r['data'], 1, 25)."$\n"; // z
echo 'Read '.stream_get_contents($r['data'], 1, 26)."$\n"; // <blank>
echo 'Read '.stream_get_contents($r['data'], 1, 0)."$\n";  // a

echo "\nTest 2\n";

$s = $dbh->prepare("select data from pdo_oci_stream_1_tab where id = 2");
$s->execute();
$r = $s->fetch();

echo 'Read '.stream_get_contents($r['data'], 5, 0)."\n";           // abcjj
echo 'Read '.stream_get_contents($r['data'], 5, 2)."\n";           // cjjjj
echo 'Read '.stream_get_contents($r['data'], 6, 1)."\n";           // bcjjjj
echo 'Read '.strlen(stream_get_contents($r['data'], -1,0))."\n";   // 30006
echo 'Read '.strlen(stream_get_contents($r['data'], 0,0))."\n";    // 0
echo 'Read '.strlen(stream_get_contents($r['data'], 0,1))."\n";    // 0
echo 'Read '.strlen(stream_get_contents($r['data'], 10,100))."\n"; // 10
echo 'Read '.stream_get_contents($r['data'], 6, 30000)."\n";       // jjjxyz
echo 'Read '.stream_get_contents($r['data'], 7, 30000)."\n";       // jjjxyz
echo 'Read '.strlen(stream_get_contents($r['data']))."\n";         // 0
echo 'Read '.strlen(stream_get_contents($r['data'], 0))."\n";      // 0
echo 'Read '.strlen(stream_get_contents($r['data'], -1))."\n";     // 0
echo 'Read '.stream_get_contents($r['data'], -1, 30000)."\n";      // jjjxyz

// Clean up

$stmtarray = array(
	"drop table pdo_oci_stream_1_tab"
);

foreach ($stmtarray as $stmt) {
	$dbh->exec($stmt);
}

?>
--EXPECTF--
Test 1
Read b$
Read cd$
Read ab$
Read abcdefghijklmnopqrstuvwxyz$
Read abcdefghijklmnopqrstuvwxyz$
Read bcdefghijklmnopqrstuvwxyz$
Read u$
Read z$
Read $
Read a$

Test 2
Read abcjj
Read cjjjj
Read bcjjjj
Read 30006
Read 0
Read 0
Read 10
Read jjjxyz
Read jjjxyz
Read 0
Read 0
Read 0
Read jjjxyz
