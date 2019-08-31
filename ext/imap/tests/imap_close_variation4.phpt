--TEST--
Test imap_close() function : usage variations - different ints as $options arg
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : bool imap_close(resource $stream_id [, int $options])
 * Description: Close an IMAP stream
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass different integers as $options arg to imap_close() to test which are
 * recognised as CL_EXPUNGE option
 */

echo "*** Testing imap_close() : usage variations ***\n";

require_once(__DIR__.'/imap_include.inc');

$inputs = array (0, 3.2768e4, -32768, PHP_INT_MAX, -PHP_INT_MAX);

$stream_id = setup_test_mailbox('', 3, $mailbox); // set up temp mailbox with 3 messages

// loop through each element of $inputs to check the behavior of imap_close()
$iterator = 1;
foreach($inputs as $input) {

	// mark added messages for deletion
	for ($i = 1; $i < 4; $i++) {
		imap_delete($stream_id, $i);
	}
	echo "\n-- Iteration $iterator --\n";
	var_dump( $check = imap_close($stream_id, $input) );

	// check that imap_close was successful, if not call imap_close and explicitly set CL_EXPUNGE
	if(false === $check) {
		imap_close($stream_id, CL_EXPUNGE);
	} else {
		// if imap_close was successful test whether CL_EXPUNGE was set by doing a message count
		$imap_stream = imap_open($mailbox, $username, $password);
		$num_msg = imap_num_msg($imap_stream);
		if ($num_msg != 0) {
			echo "CL_EXPUNGE was not set, $num_msg msgs in mailbox\n";
		} else {
			echo "CL_EXPUNGE was set\n";
		}
		// call imap_close with CL_EXPUNGE explicitly set in case mailbox not empty
		imap_close($imap_stream, CL_EXPUNGE);
	}
	$iterator++;

	// get $stream_id for next iteration
	$stream_id = imap_open($mailbox, $username, $password);
	populate_mailbox($stream_id, $mailbox, 3);

};
?>
===DONE===
--CLEAN--
<?php
require_once(__DIR__.'/clean.inc');
?>
--EXPECTF--
*** Testing imap_close() : usage variations ***
Create a temporary mailbox and add 3 msgs
.. mailbox '{%s}%s' created

-- Iteration 1 --
bool(true)
CL_EXPUNGE was not set, 3 msgs in mailbox

-- Iteration 2 --
bool(true)
CL_EXPUNGE was set

-- Iteration 3 --

Warning: imap_close(): invalid value for the flags parameter in %s on line %d
bool(false)

-- Iteration 4 --

Warning: imap_close(): invalid value for the flags parameter in %s on line %d
bool(false)

-- Iteration 5 --

Warning: imap_close(): invalid value for the flags parameter in %s on line %d
bool(false)
===DONE===
