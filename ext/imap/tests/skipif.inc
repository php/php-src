<?php
extension_loaded('imap') or die('skip imap extension not available in this build');

// Change these to make tests run successfully
$mailbox  = '{localhost/norsh}';
$username = 'webmaster@something.com';
$password = 'p4ssw0rd';
$options = OP_HALFOPEN; // this should be enough to verify server present
$retries = 0; // dont retry connect on failure

$mbox = @imap_open($mailbox, $username, $password, $options, $retries);
if (!$mbox) {
	die("skip could not connect to mailbox $mailbox");
}
imap_close($mbox);
?>
