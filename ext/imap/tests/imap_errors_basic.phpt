--TEST--
Test imap_errors() function : invalid password
--SKIPIF--
<?php
require_once __DIR__.'/setup/skipif.inc';
?>
--FILE--
<?php
echo "*** Testing imap_errors() : invalid password ***\n";
require_once __DIR__.'/setup/imap_include.inc';
$password = "bogus"; // invalid password to use in this test

echo "Issue opening with invalid password, 1 retry\n";
$mbox = imap_open(IMAP_DEFAULT_MAILBOX, IMAP_MAILBOX_USERNAME, $password, OP_READONLY, 1);

echo "List any errors\n";
var_dump(imap_errors());

?>
--EXPECTF--
*** Testing imap_errors() : invalid password ***
Issue opening with invalid password, 1 retry

Warning: imap_open(): Couldn't open stream %s in %s on line %d
List any errors
array(%d) {
  [0]=>
  string(%d) "%s"
  [1]=>
  string(%d) "%s"
}
