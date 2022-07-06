--TEST--
Test imap_fetch_overview() function : usage variations - FT_UID option
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
/*
 * Test passing a range of values into the $flags argument to imap_fetch_overview():
 * 1. values that equate to 1
 * 2. Minimum and maximum PHP values
 */

echo "*** Testing imap_fetch_overview() : usage variations ***\n";

require_once __DIR__.'/setup/imap_include.inc';

// Initialise required variables
$stream_id = setup_test_mailbox('imapfetchoverviewvar3', 1); // set up temporary mailbox with one simple message
$msg_no = 1;
$msg_uid = imap_uid($stream_id, $msg_no);

$flags = [
    '1',
    true,
    1.000000000000001,
    0.00001e5,
    245,
];

imap_check($stream_id);
foreach($flags as $option) {
    echo "\nTesting with option value:";
    var_dump($option);
    try {
        $overview = imap_fetch_overview($stream_id, $msg_uid, $option);
        if ($overview) {
            echo "imap_fetch_overview() returns an object\n";
        }
    } catch (\ValueError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

?>
--CLEAN--
<?php
$mailbox_suffix = 'imapfetchoverviewvar3';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECT--
*** Testing imap_fetch_overview() : usage variations ***
Create a temporary mailbox and add 1 msgs
New mailbox created

Testing with option value:string(1) "1"
imap_fetch_overview() returns an object

Testing with option value:bool(true)
imap_fetch_overview() returns an object

Testing with option value:float(1.000000000000001)
imap_fetch_overview() returns an object

Testing with option value:float(1)
imap_fetch_overview() returns an object

Testing with option value:int(245)
imap_fetch_overview(): Argument #3 ($flags) must be FT_UID or 0
