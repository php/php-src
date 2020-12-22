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

echo "Issue opening with invalid password, 1 attempt\n";
$mbox = @imap_open(
    IMAP_DEFAULT_MAILBOX,
    IMAP_MAILBOX_USERNAME,
    $password,
    OP_READONLY,
    1,
);

echo "List any errors\n";
var_dump(imap_errors());

?>
--EXPECT--
*** Testing imap_errors() : invalid password ***
Issue opening with invalid password, 1 attempt
List any errors
array(1) {
  [0]=>
  string(82) "Can not authenticate to IMAP server: [AUTHENTICATIONFAILED] Authentication failed."
}
