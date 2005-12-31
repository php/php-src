--TEST--
PDO PgSQL Large Objects
--SKIPIF--
<?php # vim:se ft=php:
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require dirname(__FILE__) . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(dirname(__FILE__) . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
$db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, false);

$db->exec('CREATE TABLE test (blobid integer not null primary key, bloboid OID)');

$db->beginTransaction();
$oid = $db->pgsqlLOBCreate();
try {
$stm = $db->pgsqlLOBOpen($oid);
fwrite($stm, "Hello dude\n");

$stmt = $db->prepare("INSERT INTO test (blobid, bloboid) values (?, ?)");
$stmt->bindValue(1, 1);
/* bind as LOB; the oid from the pgsql stream will be inserted instead
 * of the stream contents. Binding other streams will attempt to bind
 * as bytea, and will most likely lead to an error.
 * You can also just bind the $oid in as a string. */
$stmt->bindParam(2, $stm, PDO::PARAM_LOB);
$stmt->execute();
$stm = null;

/* Pull it out */
$stmt = $db->prepare("SELECT * from test");
$stmt->execute();
$stmt->bindColumn('bloboid', $lob, PDO::PARAM_LOB);
echo "Fetching:\n";
while (($row = $stmt->fetch(PDO::FETCH_ASSOC))) {
	var_dump($row['blobid']);
	var_dump(stream_get_contents($lob));
}
echo "Fetched!\n";
} catch (Exception $e) {
	/* catch exceptions so that we can guarantee to clean
	 * up the LOB */
	echo "Exception! at line ", $e->getLine(), "\n";
	var_dump($e->getMessage());
}

/* Now to remove the large object from the database, so it doesn't
 * linger and clutter up the storage */
$db->pgsqlLOBUnlink($oid);

--EXPECT--
Fetching:
int(1)
string(11) "Hello dude
"
Fetched!
