<?php
include_once(dirname(__FILE__) . '/imap_include.inc');

$imap_stream = imap_open($default_mailbox, $username, $password);

// delete all msgs in default mailbox, i.e INBOX
$check = imap_check($imap_stream);
for ($i = 1; $i <= $check->Nmsgs; $i++) {
	imap_delete($imap_stream, $i);
}

$mailboxes = imap_getmailboxes($imap_stream, $server, '*');

foreach($mailboxes as $value) {
	// Only delete mailboxes with our prefix
	if (preg_match('/\{.*?\}INBOX\.(.+)/', $value->name, $match) == 1) {
		if (strlen($match[1]) >= strlen($mailbox_prefix)
		&& substr_compare($match[1], $mailbox_prefix, 0, strlen($mailbox_prefix)) == 0) {
			imap_deletemailbox($imap_stream, $value->name);
		}
	}
}

imap_close($imap_stream, CL_EXPUNGE);
?>
