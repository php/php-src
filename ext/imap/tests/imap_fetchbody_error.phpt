--TEST--
Test imap_fetchbody() function : error conditions - incorrect number of args
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  :string imap_fetchbody(resource $stream_id, int $msg_no, string $section
 *          [, int $options])
 * Description: Get a specific body section
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass an incorrect number of arguments to imap_fetchbody() to test behaviour
 */

echo "*** Testing imap_fetchbody() : error conditions ***\n";
require_once(__DIR__.'/imap_include.inc');

//Test imap_fetchbody with one more than the expected number of arguments
echo "\n-- Testing imap_fetchbody() function with more than expected no. of arguments --\n";

$stream_id = setup_test_mailbox('', 1); // set up temp mailbox with 1 simple msg
$msg_no = 1;
$section = '1';
$options = FT_PEEK;
$extra_arg = 10;

var_dump( imap_fetchbody($stream_id, $msg_no, $section, $options, $extra_arg) );

// Testing imap_fetchbody with one less than the expected number of arguments
echo "\n-- Testing imap_fetchbody() function with less than expected no. of arguments --\n";

var_dump( imap_fetchbody($stream_id, $msg_no) );
?>
===DONE===
--CLEAN--
<?php
require_once(__DIR__.'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchbody() : error conditions ***

-- Testing imap_fetchbody() function with more than expected no. of arguments --
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created

Warning: imap_fetchbody() expects at most 4 parameters, 5 given in %s on line %d
NULL

-- Testing imap_fetchbody() function with less than expected no. of arguments --

Warning: imap_fetchbody() expects at least 3 parameters, 2 given in %s on line %d
NULL
===DONE===
