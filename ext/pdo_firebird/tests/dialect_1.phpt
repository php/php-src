--TEST--
PDO_Firebird: support 1 sql dialect
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc');
if (strpos(getenv('PDO_FIREBIRD_TEST_DSN'), 'dialect=1')===false) {
    die('skip: PDO_FIREBIRD_TEST_DSN must contain a string "dialect=1"');
}
?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php
require("testdb.inc");

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
$dbh->setAttribute(PDO::FB_ATTR_TIMESTAMP_FORMAT, '%Y-%m-%d %H:%M:%S');

$sql =
'SELECT
  1 as N,
  2.0 as F,
  cast(0.76 as numeric(15, 2)) as K,
  cast(\'2019-06-12\' as date) as DT
FROM RDB$DATABASE';
$query = $dbh->prepare($sql);
$query->execute();
$row = $query->fetch(\PDO::FETCH_OBJ);
var_dump($row->N);
var_dump($row->F);
var_dump($row->K);
var_dump($row->DT);

unset($query);

$dbh->exec('RECREATE TABLE test_dialect_1(K numeric(15, 2), DT date)');
$sql='INSERT INTO test_dialect_1(K, DT) values(?, ?)';
$query = $dbh->prepare($sql);
$query->execute([0.76, '2019-06-12']);
unset($query);

$sql='SELECT * FROM test_dialect_1';
$query = $dbh->prepare($sql);
$query->execute();
$row = $query->fetch(\PDO::FETCH_OBJ);
var_dump($row->K);
var_dump($row->DT);

unset($query);
unset($dbh);
echo "done\n";

?>
--CLEAN--
<?php
require 'testdb.inc';
$dbh->exec('DROP TABLE test_dialect_1');
--EXPECT--
int(1)
string(8) "2.000000"
string(4) "0.76"
string(19) "2019-06-12 00:00:00"
string(4) "0.76"
string(19) "2019-06-12 00:00:00"
done
