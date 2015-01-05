--TEST--
Test imap_fetchheader() function : usage variations - FT_UID option
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchheader(resource $stream_id, int $msg_no [, int $options])
 * Description: Get the full unfiltered header for a message 
 * Source code: ext/imap/php_imap.c
 */

/*
 * Test if FT_UID is set by passing the following as $options argument to imap_fetchheader():
 * 1. values that equate to 1
 * 2. Minimum and maximum PHP values
 */

echo "*** Testing imap_fetchheader() : usage variations ***\n";

require_once(dirname(__FILE__).'/imap_include.inc');

// Initialise required variables
$stream_id = setup_test_mailbox('', 1); // set up temporary mailbox with one simple message
$msg_no = 1;
$msg_uid = imap_uid($stream_id, $msg_no);

$options = array ('1', true, 
                  1.000000000000001, 0.00001e5, 
                  PHP_INT_MAX, -PHP_INT_MAX);

// iterate over each element of $options array to test whether FT_UID is set
$iterator = 1;
imap_check($stream_id);
foreach($options as $option) {
	echo "\n-- Iteration $iterator --\n";
	if(is_string(imap_fetchheader($stream_id, $msg_uid, $option))) {
		echo "FT_UID valid\n";
	} else {
                echo "FT_UID not valid\n";
        }
	$iterator++;
}
?>
===DONE===
--CLEAN--
<?php
require_once(dirname(__FILE__).'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchheader() : usage variations ***
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created

-- Iteration 1 --
FT_UID valid

-- Iteration 2 --
FT_UID valid

-- Iteration 3 --
FT_UID valid

-- Iteration 4 --
FT_UID valid

-- Iteration 5 --

Warning: imap_fetchheader(): invalid value for the options parameter in %s on line %d
FT_UID not valid

-- Iteration 6 --

Warning: imap_fetchheader(): invalid value for the options parameter in %s on line %d
FT_UID not valid
===DONE===
