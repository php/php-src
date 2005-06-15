--TEST--
PDO_ODBC: Bind does not convert NULL
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc'); ?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

$SQL['create'] = 'CREATE TABLE test(id INT, val VARCHAR(10))';

$DB->setAttribute(PDO_ATTR_CASE, PDO_CASE_LOWER);

require_once($PDO_TESTS . 'pdo_024.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
bind: success
bool(true)
NULL
===DONE===
