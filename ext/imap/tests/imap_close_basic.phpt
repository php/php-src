--TEST--
Test imap_close() function : basic functionality
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : bool imap_close(resource $stream_id [, int $options])
 * Description: Close an IMAP stream
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing imap_close() : basic functionality ***\n";

// include file for required variables in imap_open()
require_once(dirname(__FILE__).'/imap_include.inc');

// Initialize required variables
$stream_id = setup_test_mailbox('', 3, $mailbox); // set up temp mailbox with 3 messages
$options = CL_EXPUNGE;

// mark messages in inbox for deletion
for ($i = 1; $i < 4; $i++) {
	imap_delete($stream_id, $i);
}

// Calling imap_close() with all possible arguments
echo "\n-- Call to imap_close() with all possible arguments --\n";
var_dump( imap_close($stream_id, $options) );

// check that CL_EXPUNGE worked
$stream_id = imap_open($mailbox, $username, $password);
echo "There are now " . imap_num_msg($stream_id) . " msgs in mailbox '$mailbox'\n";

// Calling imap_close() with mandatory arguments
echo "\n-- Call to imap_close() with mandatory arguments --\n";
var_dump( imap_close($stream_id) );
?>
===DONE===
--CLEAN--
<?php
require_once(dirname(__FILE__).'/clean.inc');
?>
--EXPECTF--
*** Testing imap_close() : basic functionality ***
Create a temporary mailbox and add 3 msgs
.. mailbox '%sINBOX.phpttest' created

-- Call to imap_close() with all possible arguments --
bool(true)
There are now 0 msgs in mailbox '%sINBOX.phpttest'

-- Call to imap_close() with mandatory arguments --
bool(true)
===DONE===
