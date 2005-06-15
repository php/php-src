<?php

$USER = 'db2inst1';
$PASSWD = 'ibmdb2';
$DBNAME = 'SAMPLE';

$CONNECTION = "odbc:DSN=$DBNAME;UID=$USER;PWD=$PASSWD;";

$PDO_TESTS = dirname(__FILE__) . '/../../pdo/tests/';

?>
