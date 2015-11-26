--TEST--
Test imap_fetch_overview() function : error conditions - incorrect number of args
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : array imap_fetch_overview(resource $stream_id, int $msg_no [, int $options])
 * Description: Read an overview of the information in the headers 
 * of the given message sequence 
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass an incorrect number of arguments to imap_fetch_overview() to test behaviour
 */

echo "*** Testing imap_fetch_overview() : error conditions ***\n";

require_once(dirname(__FILE__).'/imap_include.inc');

//Test imap_fetch_overview with one more than the expected number of arguments
echo "\n-- Testing imap_fetch_overview() function with more than expected no. of arguments --\n";
$stream_id = setup_test_mailbox('', 2, $mailbox, 'notSimple'); // set up temp mailbox with 2 msgs
$msg_no = 1;
$options = FT_UID;
$extra_arg = 10;
var_dump( imap_fetch_overview($stream_id, $msg_no, $options, $extra_arg) );

// Testing imap_fetch_overview with one less than the expected number of arguments
echo "\n-- Testing imap_fetch_overview() function with less than expected no. of arguments --\n";
var_dump( imap_fetch_overview($stream_id) );
?>
===DONE===
--CLEAN--
<?php
require_once(dirname(__FILE__).'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetch_overview() : error conditions ***

-- Testing imap_fetch_overview() function with more than expected no. of arguments --
Create a temporary mailbox and add 2 msgs
.. mailbox '{%s}%s' created

Warning: imap_fetch_overview() expects at most 3 parameters, 4 given in %s on line %d
NULL

-- Testing imap_fetch_overview() function with less than expected no. of arguments --

Warning: imap_fetch_overview() expects at least 2 parameters, 1 given in %s on line %d
NULL
===DONE===
