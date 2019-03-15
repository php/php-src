--TEST--
Test imap_fetchheader() function : basic function
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : string imap_fetchheader(resource $stream_id, int $msg_no [, int $options])
 * Description: Get the full unfiltered header for a message
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing imap_fetchheader() : basic functionality ***\n";
require_once(__DIR__.'/imap_include.inc');

// Initialise all required variables
$stream_id = setup_test_mailbox('', 1, $mailbox, 'multiPart'); // setup temp mailbox with 1 msg
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
===DONE===
--CLEAN--
<?php
require_once(__DIR__.'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetchheader() : basic functionality ***
Create a temporary mailbox and add 1 msgs
.. mailbox '%s.phpttest' created

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
===DONE===
