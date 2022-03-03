--TEST--
Bug #77153 (imap_open allows to run arbitrary shell commands via mailbox parameter)
--EXTENSIONS--
imap
--CONFLICTS--
defaultmailbox
--FILE--
<?php
$payload = "echo 'BUG'> " . __DIR__ . '/__bug';
$payloadb64 = base64_encode($payload);
$server = "x -oProxyCommand=echo\t$payloadb64|base64\t-d|sh}";
@imap_open('{'.$server.':143/imap}INBOX', '', '');
// clean
imap_errors();
var_dump(file_exists(__DIR__ . '/__bug'));
?>
--EXPECT--
bool(false)
--CLEAN--
<?php
if(file_exists(__DIR__ . '/__bug')) unlink(__DIR__ . '/__bug');
?>
