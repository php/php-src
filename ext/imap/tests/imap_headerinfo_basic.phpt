--TEST--
imap_headerinfo() function : basic functionality
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

$z = imap_headerinfo($stream_id, 1);

$fields = array ('toaddress','to','fromaddress','from',
'reply_toaddress','reply_to',
	'senderaddress', 'sender',
'subject','Subject',
	'Recent','Unseen','Flagged','Answered','Deleted','Draft',
	'Msgno','MailDate','Size','udate');

echo "Check general fields\n";
foreach ($fields as $key) {
	var_dump(isset($z->$key));
}

echo "Check type\n";
var_dump($z->toaddress);
var_dump($z->fromaddress);
var_dump($z->reply_toaddress);
var_dump($z->senderaddress);
var_dump($z->subject);
var_dump($z->Subject);

if ($z->Recent == 'R' || $z->Recent == 'N' || $z->Recent == ' ') {
	echo "Recent: OK";
} else {
	echo "Recent: error: ".$z->Recent;
}
echo "\n";

if ($z->Unseen == 'U' || $z->Unseen == ' ') {
	echo "Unseen: OK";
} else {
	echo "Unseen: error: ".$z->Unseen;
}
echo "\n";

if ($z->Flagged == 'F' || $z->Flagged == ' ') {
	echo "Flagged: OK";
} else {
	echo "Flagged: error: ".$z->Flagged;
}
echo "\n";

if ($z->Answered == 'A' || $z->Answered == ' ') {
	echo "Answered: OK";
} else {
	echo "Answered: error";
}
echo "\n";

if ($z->Deleted == 'D' || $z->Deleted == ' ') {
	echo "Deleted: OK";
} else {
	echo "Deleted: error";
}
echo "\n";

if ($z->Draft == 'X' || $z->Draft == ' ') {
	echo "Draft: OK";
} else {
	echo "Draft: error";
}
echo "\n";

var_dump($z->Msgno);
var_dump($z->Size);
var_dump($z->udate);

imap_close($stream_id);

?>
--CLEAN--
<?php
require_once('clean.inc');
?>
--EXPECTF--
Create a temporary mailbox and add 1 msgs
.. mailbox '{%s}%s' created
Check general fields
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Check type
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
string(%d) "%s"
Recent: OK
Unseen: OK
Flagged: OK
Answered: OK
Deleted: OK
Draft: OK
string(%d) "%s"
string(%d) "%d"
int(%d)
