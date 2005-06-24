--TEST--
PDO_MySQL: PDO::beginTransaction / PDO::rollBack 
--SKIPIF--
<?php # vim:ft=php
die('skip no transaction support');
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_017.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
Counted 3 rows after insert.
Counted 0 rows after delete.
Counted 3 rows after rollback.
===DONE===
