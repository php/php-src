--TEST--
PDO_SQLite2: PDOStatement::getColumnMeta
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_022.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Warning: PDOStatement::getColumnMeta(): SQLSTATE[IM001]: Driver does not support this function: driver doesn't support meta data in %spdo_022.inc on line %d
bool(false)

Warning: PDOStatement::getColumnMeta(): SQLSTATE[IM001]: Driver does not support this function: driver doesn't support meta data in %spdo_022.inc on line %d
bool(false)

Warning: PDOStatement::getColumnMeta(): SQLSTATE[IM001]: Driver does not support this function: driver doesn't support meta data in %spdo_022.inc on line %d
bool(false)

Warning: PDOStatement::getColumnMeta(): SQLSTATE[IM001]: Driver does not support this function: driver doesn't support meta data in %spdo_022.inc on line %d
bool(false)
===DONE===
