--TEST--
Test imap_fetchheader() function : basic function
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php
echo "*** Testing imap_fetchheader() : basic functionality ***\n";
require_once(__DIR__.'/setup/imap_include.inc');

// Initialise all required variables
$stream_id = setup_test_mailbox('imapfetchheaderbasic', 1, $mailbox, false); // setup temp mailbox with 1 msg
$msg_no = 1;
$options = array('FT_UID' => FT_UID, 'FT_INTERNAL' => FT_INTERNAL,
                 'FT_PREFETCHTEXT' => FT_PREFETCHTEXT);

// Calling imap_fetchheader() with all possible arguments
echo "\n-- All possible arguments --\n";
foreach ($options as $key => $option) {
    echo "-- Option is $key --\n";
    if ($key == 'FT_UID') {
        $msg_uid = imap_uid($stream_id, $msg_no);
        var_dump(imap_fetchheader($stream_id, $msg_uid, $option));
    } else {
        var_dump(imap_fetchheader($stream_id, $msg_no, $option));
    }
}

// Calling imap_fetchheader() with mandatory arguments
echo "\n-- Mandatory arguments --\n";
var_dump( imap_fetchheader($stream_id, $msg_no) );
?>
--CLEAN--
<?php
$mailbox_suffix = 'imapfetchheaderbasic';
require_once(__DIR__.'/setup/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchheader() : basic functionality ***
Create a temporary mailbox and add 1 msgs
New mailbox created

-- All possible arguments --
-- Option is FT_UID --
string(%d) "From: foo@anywhere.com
Subject: Test msg 1
To: %s
MIME-Version: 1.0
Content-Type: %s; %s

"
-- Option is FT_INTERNAL --
string(%d) "From: foo@anywhere.com
Subject: Test msg 1
To: %s
MIME-Version: 1.0
Content-Type: %s; %s

"
-- Option is FT_PREFETCHTEXT --
string(%d) "From: foo@anywhere.com
Subject: Test msg 1
To: %s
MIME-Version: 1.0
Content-Type: %s; %s

"

-- Mandatory arguments --
string(%d) "From: foo@anywhere.com
Subject: Test msg 1
To: %s
MIME-Version: 1.0
Content-Type: %s; %s

"
