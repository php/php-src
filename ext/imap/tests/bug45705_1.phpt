--TEST--
Bug #45705 test #1 (imap rfc822_parse_adrlist() modifies passed address parameter)
--SKIPIF--
<?php
	if (!extension_loaded("imap")) { 
		die("skip imap extension not available");  
	}
?>
--FILE--
<?php

$address = 'John Doe <john@example.com>';
var_dump($address);
imap_rfc822_parse_adrlist($address, null);
var_dump($address);

?>
--EXPECT--
string(27) "John Doe <john@example.com>"
string(27) "John Doe <john@example.com>"
