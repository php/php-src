--TEST--
PDO_Firebird: persistent connect test
--EXTENSIONS--
pdo_firebird
--SKIPIF--
<?php require('skipif.inc'); ?>
--XLEAK--
A bug in firebird causes a memory leak when calling `isc_attach_database()`.
See https://github.com/FirebirdSQL/firebird/issues/7849
--FILE--
<?php

/**
 * Omit the case where the connection is broken when it checks liveness and
 * it has to reconnect, as it is very difficult to reproduce the situation.
 */

require("testdb.inc");
unset($dbh);

$connIds = [];

foreach (['First', 'Second'] as $times) {
    $dbh = new PDO(
        PDO_FIREBIRD_TEST_DSN,
        PDO_FIREBIRD_TEST_USER,
        PDO_FIREBIRD_TEST_PASS,
        [
            PDO::ATTR_PERSISTENT => true,
        ],
    );
    $stmt = $dbh->query('SELECT CURRENT_CONNECTION FROM RDB$DATABASE');
    $connId = $stmt->fetchColumn();
    $connIds[] = $connId;
    echo "{$times} connection ID: {$connId}\n";

    unset($dbh);
    unset($stmt);
    unset($connID);
}

echo $connIds[0] === $connIds[1] ? "Same ID.\n" : "Different ID\n";
?>
--EXPECTF--
First connection ID: %d
Second connection ID: %d
Same ID.
