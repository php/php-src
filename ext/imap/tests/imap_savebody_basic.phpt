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
echo "Checking with no parameters\n";
imap_savebody();

echo  "Checking with incorrect parameter type\n";
imap_savebody('');
imap_savebody(false);

require_once(__DIR__.'/imap_include.inc');
$stream_id = setup_test_mailbox('', 1);

imap_savebody($stream_id);

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
Checking with no parameters

Warning: imap_savebody() expects at least 3 parameters, 0 given in %s on line %d
Checking with incorrect parameter type

Warning: imap_savebody() expects at least 3 parameters, 1 given in %s on line %d

Warning: imap_savebody() expects at least 3 parameters, 1 given in %s on line %d
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created

Warning: imap_savebody() expects at least 3 parameters, 1 given in %s on line %d
bool(true)
Size: %d
bool(true)
Size: %d
