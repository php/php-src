--TEST--
PDO_SQLite2: Bind does not convert NULL
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_024.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
bind: success
bool(true)
NULL
===DONE===
