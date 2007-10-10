--TEST--
mysqli_options()
--SKIPIF--
<?php 
require_once('skipif.inc');
require_once('skipifemb.inc'); 
require_once('skipifconnectfailure.inc');

die("skip - STUB - TODO - this is a stub to remind me that we should also actually test the options");
?>
<?php require_once('skipifemb.inc'); ?>
--FILE--
<?php
	/* see mysqli.c for details */
	include "connect.inc";
	print "done!";
?>
--EXPECTF--
done!
