<?php

$DATABASE = '/opt/firebird/test.gdb';
$CONNECTION = "firebird:dbname=$DATABASE";
$USER = 'SYSDBA';
$PASSWORD = 'password';

$PDO_TESTS = dirname(__FILE__) . '/../../pdo/tests/';

?>
