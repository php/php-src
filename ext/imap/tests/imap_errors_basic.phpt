--TEST--
Test imap_errors() function : anonymous user not supported
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once __DIR__.'/setup/skipif.inc';
?>
--FILE--
<?php
echo "*** Testing imap_errors() : anonymous user not supported ***\n";
require_once __DIR__.'/setup/imap_include.inc';

$mbox = @imap_open(IMAP_DEFAULT_MAILBOX, IMAP_MAILBOX_USERNAME, IMAP_MAILBOX_PASSWORD, OP_ANONYMOUS);

echo "List any errors\n";
var_dump(imap_errors());

?>
--EXPECTF--
*** Testing imap_errors() : anonymous user not supported ***
List any errors
array(1) {
  [0]=>
  string(%d) "%s"
}
