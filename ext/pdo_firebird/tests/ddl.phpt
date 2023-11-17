--TEST--
PDO_Firebird: DDL/transactions
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

require("testdb.inc");

$dbh->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
@$dbh->exec('DROP TABLE test_ddl');
@$dbh->exec('DROP GENERATOR gen_test_ddl_id');
@$dbh->exec('DROP TRIGGER test_ddl_bi');

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

@$dbh->exec('DROP TABLE test_ddl');
@$dbh->exec('DROP GENERATOR gen_test_ddl_id');
@$dbh->exec('DROP TRIGGER test_ddl_bi');

unset($dbh);
echo "done\n";
?>
--EXPECT--
int(1)
int(1)
int(0)
done
