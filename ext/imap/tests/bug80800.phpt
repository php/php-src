--TEST--
Bug #80800: imap_open() fails when the flags parameter includes CL_EXPUNGE
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once __DIR__.'/setup/imap_include.inc';

$mail_box = imap_open(IMAP_DEFAULT_MAILBOX, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD, flags: CL_EXPUNGE);
imap_close($mail_box);

echo 'Connected without any issues', "\n";

?>
--EXPECT--
Connected without any issues
