--TEST--
PostgreSQL notice function
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
include("notice.inc");
?>
--EXPECT--
NOTICE:  BEGIN: already a transaction in progress

NOTICE:  BEGIN: already a transaction in progress
pg_last_notice() is Ok
