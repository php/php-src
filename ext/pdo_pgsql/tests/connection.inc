<?php

$HOST = 'localhost';
$PORT = 5432;
$USER = 'root';
$PASSWD = '';
$DBNAME = 'test';

$CONNECTION = "pgsql:host=$HOST port=$PORT dbname=$DBNAME user=$USER password=$PASSWD";

$PDO_TESTS = dirname(__FILE__) . '/../../pdo/tests/';

?>
