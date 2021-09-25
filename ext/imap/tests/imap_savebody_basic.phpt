--TEST--
imap_savebody() function : basic functionality
--CREDITS--
Olivier Doucet
--EXTENSIONS--
imap
--SKIPIF--
<?php
require_once(__DIR__.'/setup/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__.'/setup/imap_include.inc');
$stream_id = setup_test_mailbox('imapsavebodybasic', 1);

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
$mailbox_suffix = 'imapsavebodybasic';
require_once(__DIR__ . '/setup/clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 1 msgs
New mailbox created
bool(true)
Size: %d
bool(true)
Size: %d
