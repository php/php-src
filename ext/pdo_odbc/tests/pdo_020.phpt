--TEST--
PDO_ODBC: PDOStatement::columnCount
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_020.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Counted 2 columns after select1.
Counted 3 columns after select2.
Counted 1 columns after select3.
===DONE===
