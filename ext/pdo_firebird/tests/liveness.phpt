--TEST--
PDO_Firebird: Check persistent conection is alive
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
    require("testdb.inc");

    @$dbh->exec('DROP TABLE testz');
    $dbh->exec('CREATE TABLE testz (A VARCHAR(10))');
    $dbh->exec("INSERT INTO testz VALUES ('A')");
    $dbh->commit();

    $dsn = getenv('PDO_FIREBIRD_TEST_DSN');
    $user = getenv('PDO_FIREBIRD_TEST_USER');
    $pass = getenv('PDO_FIREBIRD_TEST_PASS');

    //first query is to make php create and hold a connection
    $dbh_p = new pdo($dsn, $user, $pass, array(PDO::ATTR_PERSISTENT => true));

    //execute a query for check if the connection is alive
    $stmt = $dbh_p->prepare('SELECT * FROM testz');
    $stmt->execute();
    $stmt->fetchAll();

    //close the connection
    $dbh_p = null;
    gc_collect_cycles();

    //Force firebird close all connection in database execept the $dbh
    $dbh->query('DELETE FROM MON$ATTACHMENTS WHERE MON$ATTACHMENT_ID <> CURRENT_CONNECTION');

    //pick connectin again
    $dbh_p2 = new pdo($dsn, $user, $pass, array(PDO::ATTR_PERSISTENT => true));

    //run a query to make php raiser the error
    $stmt2 = $dbh_p2->prepare('SELECT * FROM testz');
    $stmt2->execute();
    $result = $stmt2->fetchAll();

    echo "done";
    ?>
--EXPECT--
done
