--TEST--
Test imap_createmailbox() function : basic functionality
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : bool imap_createmailbox  ( resource $imap_stream  , string $mailbox  )
 * Description: Creates a new mailbox specified by mailbox .
 * Source code: ext/imap/php_imap.c
 */

echo "*** Testing imap_createmailbox() : basic functionality ***\n";

require_once(__DIR__.'/imap_include.inc');

$imap_stream = imap_open($default_mailbox, $username, $password) or
	die("Cannot connect to mailbox $default_mailbox: " . imap_last_error());

$newname = "phpnewbox";

echo "Newname will be '$newname'\n";

$newbox = imap_utf7_encode($server.$newname);
if (imap_createmailbox($imap_stream, $newbox)) {

	echo "Add a couple of msgs to '$newname' mailbox\n";
	populate_mailbox($imap_stream, $newbox, 2);

    $status = imap_status($imap_stream, $newbox, SA_ALL);
    if ($status) {
        echo "Your new mailbox '$newname' has the following status:\n";
        echo "Messages:    " . $status->messages    . "\n";
        echo "Recent:      " . $status->recent      . "\n";
        echo "Unseen:      " . $status->unseen      . "\n";
        echo "UIDnext:     " . $status->uidnext     . "\n";
        echo "UIDvalidity: " . $status->uidvalidity . "\n";

    } else {
        echo "imap_status on new mailbox failed: " . imap_last_error() . "\n";
    }

    if (imap_deletemailbox($imap_stream, $newbox)) {
        echo "Mailbox '$newname' removed to restore initial state\n";
    } else {
        echo "imap_deletemailbox on new mailbox failed: " . implode("\n", imap_errors()) . "\n";
    }

} else {
    echo "could not create new mailbox: " . implode("\n", imap_errors()) . "\n";
}

imap_close($imap_stream);

?>
===Done===
--EXPECTF--
*** Testing imap_createmailbox() : basic functionality ***
Newname will be 'phpnewbox'
Add a couple of msgs to 'phpnewbox' mailbox
Your new mailbox 'phpnewbox' has the following status:
Messages:    2
Recent:      2
Unseen:      2
UIDnext:     %d
UIDvalidity: %d
Mailbox 'phpnewbox' removed to restore initial state
===Done===
