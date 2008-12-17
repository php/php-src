--TEST--
Test imap_fetchbody() function : basic functionality
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchbody(resource $stream_id, int $msg_no, string $section 
 *           [, int $options])
 * Description: Get a specific body section 
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing imap_fetchbody() : basic functionality ***\n";
require_once(dirname(__FILE__).'/imap_include.inc');

// Initialise all required variables

// set up mailbox with one message
$stream_id = setup_test_mailbox('', 3, $mailbox, 'notSimple');
$section = '2';

// Calling imap_fetchbody() with all possible arguments
echo "\n-- All possible arguments --\n";

echo "Option == FD_UID\n";
$msg_uid = imap_uid($stream_id, 1);
var_dump( imap_fetchbody($stream_id, $msg_uid, $section, FT_UID) );

echo "Option == FD_PEEK\n";
var_dump( imap_fetchbody($stream_id, 2, $section, FT_PEEK) );
$overview = imap_fetch_overview($stream_id, 2);
echo "Seen Flag: ";
var_dump( $overview[0]->seen );

echo "option == FD_INTERNAL\n";
var_dump( imap_fetchbody($stream_id, 3, $section, FT_INTERNAL) );

// Calling imap_fetchbody() with mandatory arguments
echo "\n-- Mandatory arguments --\n";
var_dump( imap_fetchbody($stream_id, 1, $section) );
$overview = imap_fetch_overview($stream_id, 1);
echo "Seen Flag: ";
var_dump( $overview[0]->seen );

?>
===DONE===
--CLEAN--
<?php
require_once(dirname(__FILE__).'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchbody() : basic functionality ***
Create a temporary mailbox and add 3 msgs
.. mailbox '{localhost/norsh}INBOX.phpttest' created

-- All possible arguments --
Option == FD_UID
string(36) "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy"
Option == FD_PEEK
string(36) "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy"
Seen Flag: int(%d)
option == FD_INTERNAL
string(36) "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy"

-- Mandatory arguments --
string(36) "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy"
Seen Flag: int(%d)
===DONE===