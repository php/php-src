--TEST--
PDO_Firebird: auto commit
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php
/* Part of the error messages probably vary depending on the version of Firebird,
 * so it won't check them in detail. */

require("testdb.inc");
$table = "autocommit_pdo_firebird";

echo "========== in auto commit mode ==========\n";
echo "auto commit mode ON\n";
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, true);

echo "create table and insert\n";
$dbh->exec("CREATE TABLE {$table} (val INT)");
$dbh->exec("INSERT INTO {$table} VALUES (35)");

echo "new connection\n";
unset($dbh);
$dbh = new PDO(PDO_FIREBIRD_TEST_DSN, PDO_FIREBIRD_TEST_USER, PDO_FIREBIRD_TEST_PASS);

$r = $dbh->query("SELECT * FROM {$table}");
var_dump($r->fetchAll());

echo "========== not in auto commit mode ==========\n";
echo "auto commit mode OFF\n";
$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT, false);

echo "insert, expect error\n";
try {
    $dbh->exec("INSERT INTO {$table} VALUES (35)");
} catch (PDOException $e) {
    echo $e->getMessage()."\n\n";
}

echo "select, expect error\n";
try {
    $r = $dbh->query("SELECT * FROM {$table}");
} catch (PDOException $e) {
    echo $e->getMessage()."\n\n";
}

echo "done!";
?>
--CLEAN--
<?php
require 'testdb.inc';
@$dbh->exec("DROP TABLE autocommit_pdo_firebird");
?>
--EXPECTF--
========== in auto commit mode ==========
auto commit mode ON
create table and insert
new connection
array(1) {
  [0]=>
  array(2) {
    ["VAL"]=>
    int(35)
    [0]=>
    int(35)
  }
}
========== not in auto commit mode ==========
auto commit mode OFF
insert, expect error
SQLSTATE[08003]: Connection does not exist: %s

select, expect error
SQLSTATE[08003]: Connection does not exist: %s

done!
