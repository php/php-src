--TEST--
Test imap_fetchbody() function : basic functionality
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
/*           [, int $options])
 * Description: Get a specific body section
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing imap_fetchbody() : basic functionality ***\n";
require_once(__DIR__.'/setup/imap_include.inc');

// Initialise all required variables

// set up mailbox with one message
$stream_id = setup_test_mailbox('imapfetchbodybasic', 1, $mailbox, false);

$msg_no = 1;
$section = '2';
$options = array ('FT_UID' => FT_UID, 'FT_PEEK' => FT_PEEK, 'FT_INTERNAL' => FT_INTERNAL);

// Calling imap_fetchbody() with all possible arguments
echo "\n-- All possible arguments --\n";
foreach ($options as $key => $option) {
    echo "-- Option is $key --\n";
    switch ($key) {

        case 'FT_UID';
        $msg_uid = imap_uid($stream_id, $msg_no);
        var_dump( imap_fetchbody($stream_id, $msg_uid, $section, $option) );
        break;

        case 'FT_PEEK';
        var_dump( imap_fetchbody($stream_id, $msg_no, $section, $option) );
        $overview = imap_fetch_overview($stream_id, 1);
        echo "Seen Flag: ";
        var_dump( $overview[0]->seen );
        break;

        case 'FT_INTERNAL';
        var_dump( imap_fetchbody($stream_id, $msg_no, $section, $option) );
        break;

    }
}

// Calling imap_fetchbody() with mandatory arguments
echo "\n-- Mandatory arguments --\n";
var_dump( imap_fetchbody($stream_id, $msg_no, $section) );
$overview = imap_fetch_overview($stream_id, 1);
echo "Seen Flag: ";
var_dump( $overview[0]->seen );
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapfetchbodybasic';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchbody() : basic functionality ***
Create a temporary mailbox and add 1 msgs
New mailbox created

-- All possible arguments --
-- Option is FT_UID --
string(36) "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy"
-- Option is FT_PEEK --
string(36) "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy"
Seen Flag: int(%d)
-- Option is FT_INTERNAL --
string(36) "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy"

-- Mandatory arguments --
string(36) "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy"
Seen Flag: int(%d)
