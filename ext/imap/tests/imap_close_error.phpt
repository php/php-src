--TEST--
Test imap_close() function : error conditions - incorrect number of args
--SKIPIF--
<?php
require_once (dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : bool imap_close(resource $stream_id [, int $options])
 * Description: Close an IMAP stream 
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass an incorrect number of arguments to imap_close() to test behaviour
 */

echo "*** Testing imap_close() : error conditions ***\n";
require_once(dirname(__FILE__).'/imap_include.inc');

// Zero arguments
echo "\n-- Testing imap_close() function with Zero arguments --\n";
var_dump( imap_close() );

//Test imap_close with one more than the expected number of arguments
echo "\n-- Testing imap_close() function with more than expected no. of arguments --\n";
$stream_id = imap_open($server, $username, $password);
$options = CL_EXPUNGE;
$extra_arg = 10;
var_dump( imap_close($stream_id, $options, $extra_arg) );
?>
===DONE===
--EXPECTF--
*** Testing imap_close() : error conditions ***

-- Testing imap_close() function with Zero arguments --

Warning: imap_close() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing imap_close() function with more than expected no. of arguments --

Warning: imap_close() expects at most 2 parameters, 3 given in %s on line %d
NULL
===DONE===
