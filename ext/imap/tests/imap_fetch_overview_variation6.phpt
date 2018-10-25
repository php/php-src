--TEST--
Test imap_fetch_overview() function : usage variations - multipart message
--SKIPIF--
<?php
require_once(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php
/* Prototype  : array imap_fetch_overview(resource $stream_id, int $msg_no [, int $options])
 * Description: Read an overview of the information in the headers of the given message sequence
 * Source code: ext/imap/php_imap.c
 */

/*
 * Pass a multipart message to imap_fetch_overview() to test the contents of returned array
 */

echo "*** Testing imap_fetch_overview() : usage variations ***\n";

require_once(dirname(__FILE__).'/imap_include.inc');

$stream_id = setup_test_mailbox('', 0, $mailbox); // setup temp mailbox
create_multipart_message($stream_id, $mailbox);

// refresh msg numbers
imap_check($stream_id);
$msg_no = 1;

$a = imap_fetch_overview($stream_id, $msg_no);
echo "\n--> Object #1\n";
displayOverviewFields($a[0]);




/**
 * Create a multipart message with subparts
 *
 * @param resource $imap_stream
 * @param string $mailbox
 */
function create_multipart_message($imap_stream, $mailbox) {
	global $users, $domain;
	$envelope["from"]= "foo@anywhere.com";
	$envelope["to"]  = "$users[0]@$domain";
	$envelope["subject"] = "Test msg 1";

	$part1["type"] = TYPEMULTIPART;
	$part1["subtype"] = "mixed";

	$part2["type"] = TYPETEXT;
	$part2["subtype"] = "plain";
	$part2["description"] = "imap_mail_compose() function";
	$part2["contents.data"] = "message 1:xxxxxxxxxxxxxxxxxxxxxxxxxx";

	$part3["type"] = TYPETEXT;
	$part3["subtype"] = "plain";
	$part3["description"] = "Example";
	$part3["contents.data"] = "message 2:yyyyyyyyyyyyyyyyyyyyyyyyyy";

	$file_handle = fopen(__FILE__, 'r+');
	$file_size = 1;

	$part4["type"] = TYPEAPPLICATION;
	$part4["encoding"] = ENCBASE64;
	$part4["subtype"] = "octet-stream";
	$part4["description"] = 'Test';
	$part4['disposition.type'] = 'attachment';
	$part4['disposition'] = array ('filename' => 'Test');
	$part4['type.parameters'] = array('name' => 'Test');
	$part4["contents.data"] = base64_encode(fread($file_handle, 1));

	$body[1] = $part1;
	$body[2] = $part2;
	$body[3] = $part3;
	$body[4] = $part4;

	$msg = imap_mail_compose($envelope, $body);

	if (imap_append($imap_stream, $mailbox, $msg) === false) {
		echo imap_last_error() . "\n";
		echo "TEST FAILED : could not append new message to mailbox '$mailbox'\n";
		exit;
	}
}

?>
===DONE===
--CLEAN--
<?php
require_once(dirname(__FILE__).'/clean.inc');
?>
--EXPECTF--
*** Testing imap_fetch_overview() : usage variations ***
Create a temporary mailbox and add 0 msgs
.. mailbox '{%s}%s' created

--> Object #1
size is %d
uid is %d
msgno is 1
recent is %d
flagged is 0
answered is 0
deleted is 0
seen is 0
draft is 0
udate is OK
===DONE===
