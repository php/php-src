--TEST--
PDO_ODBC: PDOStatement::execute with parameter markers.
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_021.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
There are 6 rows in the table.
There are 12 rows in the table.
===DONE===
