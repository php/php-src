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

    @$dbh->exec('DROP TABLE ddl');
    @$dbh->exec('DROP GENERATOR gen_ddl_id');
    @$dbh->exec('DROP TRIGGER ddl_bi');

    $dbh->exec("CREATE TABLE ddl (id INT NOT NULL PRIMARY KEY, text BLOB SUB_TYPE 1)");
    $dbh->exec("CREATE GENERATOR gen_ddl_id");
    $dbh->exec("CREATE TRIGGER ddl_bi FOR ddl BEFORE INSERT AS
        BEGIN IF (NEW.id IS NULL) THEN NEW.id=GEN_ID(gen_ddl_id,1); END");

    $dbh->setAttribute(PDO::ATTR_AUTOCOMMIT,0);

    $dbh->beginTransaction();
    var_dump($dbh->exec("INSERT INTO ddl (text) VALUES ('bla')"));
    var_dump($dbh->exec("UPDATE ddl SET text='blabla'"));
    $dbh->rollback();

    $dbh->beginTransaction();
    var_dump($dbh->exec("DELETE FROM ddl"));
    $dbh->commit();

    unset($dbh);
    echo "done\n";

?>
--EXPECT--
int(1)
int(1)
int(0)
done
