<?php
extension_loaded('imap') or die('skip imap extension not available in this build');

include __DIR__ . '/imap_include.inc';

$options = OP_HALFOPEN; // this should be enough to verify server present
$retries = 0; // don't retry connect on failure

$mbox = @imap_open(IMAP_SERVER, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD, $options, $retries);
if (!$mbox) {
    die("skip could not connect to mailbox " . IMAP_SERVER);
}
imap_close($mbox);
