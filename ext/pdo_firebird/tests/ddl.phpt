--TEST--
PDO_Firebird: DDL/transactions
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--ENV--
LSAN_OPTIONS=detect_leaks=0
--FILE--
<?php

require("testdb.inc");

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

$dbh->exec("CREATE TABLE test_ddl (id INT NOT NULL PRIMARY KEY, text BLOB SUB_TYPE 1)");
$dbh->exec("CREATE GENERATOR gen_test_ddl_id");
$dbh->exec("CREATE TRIGGER test_ddl_bi FOR test_ddl BEFORE INSERT AS
    BEGIN IF (NEW.id IS NULL) THEN NEW.id=GEN_ID(gen_test_ddl_id,1); END");

$dbh->setAttribute(PDO::ATTR_AUTOCOMMIT,0);

$dbh->beginTransaction();
var_dump($dbh->exec("INSERT INTO test_ddl (text) VALUES ('bla')"));
var_dump($dbh->exec("UPDATE test_ddl SET text='blabla'"));
$dbh->rollback();

$dbh->beginTransaction();
var_dump($dbh->exec("DELETE FROM test_ddl"));
$dbh->commit();

unset($dbh);
echo "done\n";
?>
-CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
require("testdb.inc");
@$dbh->exec('DROP TABLE test_ddl');
@$dbh->exec('DROP GENERATOR gen_test_ddl_id');
@$dbh->exec('DROP TRIGGER test_ddl_bi');

?>
--EXPECT--
int(1)
int(1)
int(0)
done
