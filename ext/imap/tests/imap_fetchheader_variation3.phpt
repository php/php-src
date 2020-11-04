--TEST--
Test imap_fetchheader() function : usage variations - FT_UID option
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
/*
 * Test if FT_UID is set by passing the following as $flags argument to imap_fetchheader():
 * 1. values that equate to 1
 * 2. Minimum and maximum PHP values
 */

echo "*** Testing imap_fetchheader() : usage variations ***\n";

require_once(__DIR__.'/setup/imap_include.inc');

// Initialise required variables
$stream_id = setup_test_mailbox('imapfetchheadervar3', 1); // set up temporary mailbox with one simple message
$msg_no = 1;
$msg_uid = imap_uid($stream_id, $msg_no);

$flags = array ('1', true,
                  1.000000000000001, 0.00001e5,
                  PHP_INT_MAX, -PHP_INT_MAX);

// iterate over each element of $flags array to test whether FT_UID is set
$iterator = 1;
imap_check($stream_id);
foreach($flags as $option) {
    echo "\n-- Iteration $iterator --\n";
    try {
        if (is_string(imap_fetchheader($stream_id, $msg_uid, $option))) {
            echo "FT_UID valid\n";
        } else {
            echo "FT_UID not valid\n";
        }
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
    $iterator++;
}
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapfetchheadervar3';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECT--
*** Testing imap_fetchheader() : usage variations ***
Create a temporary mailbox and add 1 msgs
New mailbox created

-- Iteration 1 --
FT_UID valid

-- Iteration 2 --
FT_UID valid

-- Iteration 3 --
FT_UID valid

-- Iteration 4 --
FT_UID valid

-- Iteration 5 --
imap_fetchheader(): Argument #3 ($flags) must be a bitmask of FT_UID, FT_PREFETCHTEXT, and FT_INTERNAL

-- Iteration 6 --
imap_fetchheader(): Argument #3 ($flags) must be a bitmask of FT_UID, FT_PREFETCHTEXT, and FT_INTERNAL
