--TEST--
Test imap_close() function : usage variations - different ints as $flags arg
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
/*
 * Pass different integers as $flags arg to imap_close() to test which are
 * recognised as CL_EXPUNGE option
 */

echo "*** Testing imap_close() : usage variations ***\n";

require_once(__DIR__.'/setup/imap_include.inc');

$inputs = array (0, 3.2768e4, -32768, PHP_INT_MAX, -PHP_INT_MAX);

$stream_id = setup_test_mailbox('imapclosevar4', 3, $mailbox); // set up temp mailbox with 3 messages

// loop through each element of $inputs to check the behavior of imap_close()
$iterator = 1;
foreach($inputs as $input) {

    // mark added messages for deletion
    for ($i = 1; $i < 4; $i++) {
        imap_delete($stream_id, $i);
    }
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump( $check = imap_close($stream_id, $input) );
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
        $check = false;
    }

    // check that imap_close was successful, if not call imap_close and explicitly set CL_EXPUNGE
    if(false === $check) {
        imap_close($stream_id, CL_EXPUNGE);
    } else {
        // if imap_close was successful test whether CL_EXPUNGE was set by doing a message count
        $imap_stream = imap_open($mailbox, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD);
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
    $stream_id = imap_open($mailbox, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD);
    populate_mailbox($stream_id, $mailbox, 3);

};
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapclosevar4';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECT--
*** Testing imap_close() : usage variations ***
Create a temporary mailbox and add 3 msgs
New mailbox created

-- Iteration 1 --
bool(true)
CL_EXPUNGE was not set, 3 msgs in mailbox

-- Iteration 2 --
bool(true)
CL_EXPUNGE was set

-- Iteration 3 --
imap_close(): Argument #2 ($flags) must be CL_EXPUNGE or 0

-- Iteration 4 --
imap_close(): Argument #2 ($flags) must be CL_EXPUNGE or 0

-- Iteration 5 --
imap_close(): Argument #2 ($flags) must be CL_EXPUNGE or 0
