--TEST--
Test imap_fetchheader() function : error conditions - incorrect number of args
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchheader(resource $stream_id, int $msg_no [, int $options])
 * Description: Get the full unfiltered header for a message
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass an incorrect number of arguments to imap_fetchheader() to test behaviour
 */

echo "*** Testing imap_fetchheader() : error conditions ***\n";
require_once(__DIR__.'/imap_include.inc');

//Test imap_fetchheader with one more than the expected number of arguments
echo "\n-- Testing imap_fetchheader() function with more than expected no. of arguments --\n";

$stream_id = imap_open($server, $username, $password);
$msg_no = 10;
$options = 10;
$extra_arg = 10;
var_dump( imap_fetchheader($stream_id, $msg_no, $options, $extra_arg) );

// Testing imap_fetchheader with one less than the expected number of arguments
echo "\n-- Testing imap_fetchheader() function with less than expected no. of arguments --\n";
var_dump( imap_fetchheader($stream_id) );

imap_close($stream_id);
?>
===DONE===
--EXPECTF--
*** Testing imap_fetchheader() : error conditions ***

-- Testing imap_fetchheader() function with more than expected no. of arguments --

Warning: imap_fetchheader() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing imap_fetchheader() function with less than expected no. of arguments --

Warning: imap_fetchheader() expects at least 2 parameters, 1 given in %s on line %d
NULL
===DONE===
