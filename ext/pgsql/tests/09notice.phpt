--TEST--
PostgreSQL notice function
--SKIPIF--
<?php

include("skipif.inc");

_skip_lc_messages();

?>
--INI--
pgsql.log_notice=1
pgsql.ignore_notices=0
--FILE--
<?php
include 'config.inc';
include 'lcmess.inc';

$db = pg_connect($conn_str);

_set_lc_messages();

pg_query($db, "BEGIN;");
pg_query($db, "BEGIN;");

$msg = pg_last_notice($db);
if ($msg === FALSE) {
	echo "Cannot find notice message in hash\n";
	var_dump($msg);
}
echo $msg."\n";
echo "pg_last_notice() is Ok\n";

?>
--EXPECTF--
Notice: pg_query(): %s already a transaction in progress in %s on line %d
%s already a transaction in progress
pg_last_notice() is Ok
