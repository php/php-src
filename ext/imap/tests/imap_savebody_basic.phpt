--TEST--
imap_savebody() function : basic functionality
--CREDITS--
Olivier Doucet
--SKIPIF--
<?php
require_once(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/imap_include.inc');
$stream_id = setup_test_mailbox('', 1);

$file = __DIR__.'/tmpsavebody.txt';

//with URL
$z = imap_savebody($stream_id, $file, 1);
var_dump($z);
echo "Size: ".filesize($file)."\n";

//With FOPEN
$fp = fopen($file, 'w');
$z = imap_savebody($stream_id, $fp, 1);
fclose($fp);
var_dump($z);
echo "Size: ".filesize($file)."\n";

imap_close($stream_id);
?>
--CLEAN--
<?php
@unlink(__DIR__.'/tmpsavebody.txt');
require_once('clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 1 msgs
.. mailbox '{127.0.0.1:143/norsh}INBOX.phpttest' created
bool(true)
Size: %d
bool(true)
Size: %d
