--TEST--
Bug #80220 (imap_mail_compose() may leak memory) - message/rfc822 regression
--SKIPIF--
<?php
if (!extension_loaded('imap')) die('skip imap extension not available');
?>
--FILE--
<?php
$bodies = [[
    'type' => TYPEMESSAGE,
    'subtype' => 'RFC822',
], [
    'contents.data' => 'asd',
]];
var_dump(imap_mail_compose([], $bodies));

$bodies = [[
    'type' => TYPEMESSAGE,
], [
    'contents.data' => 'asd',
]];
var_dump(imap_mail_compose([], $bodies));
?>
--EXPECT--
string(53) "MIME-Version: 1.0
Content-Type: MESSAGE/RFC822


"
string(53) "MIME-Version: 1.0
Content-Type: MESSAGE/RFC822


"
