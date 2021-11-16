--TEST--
imap_rfc822_write_address() : basic functionality
--EXTENSIONS--
imap
--FILE--
<?php
var_dump(imap_rfc822_write_address('me', 'example.com', 'My Name'));
?>
--EXPECT--
string(24) "My Name <me@example.com>"
