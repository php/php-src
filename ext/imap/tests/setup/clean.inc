<?php
require_once 'imap_include.inc';

if (!isset($mailbox_suffix)) { throw new Exception("No suffix provided"); }

$imap_stream = imap_open(IMAP_DEFAULT_MAILBOX, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD);

// delete all msgs in default mailbox, i.e INBOX
$check = imap_check($imap_stream);
for ($i = 1; $i <= $check->Nmsgs; $i++) {
    imap_delete($imap_stream, $i);
}


$mailboxes = imap_getmailboxes($imap_stream, IMAP_SERVER, '*');

if (!is_array($mailbox_suffix)) {
    $mailbox_suffixes = [$mailbox_suffix];
} else {
    $mailbox_suffixes = $mailbox_suffix;
}

foreach ($mailbox_suffixes as $mailbox_suffix) {
    foreach($mailboxes as $value) {
        // Only delete mailbox with our prefix (+ optional test suffix)
        if (preg_match('/\{.*?\}INBOX\.' . IMAP_MAILBOX_PHPT_PREFIX . $mailbox_suffix .'$/', $value->name, $match) == 1) {
            imap_deletemailbox($imap_stream, $value->name);
        }
    }
}

imap_close($imap_stream, CL_EXPUNGE);
