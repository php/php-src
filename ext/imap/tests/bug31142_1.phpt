--TEST--
Bug #31142 test #1 (imap_mail_compose() generates incorrect output)
--EXTENSIONS--
imap
--FILE--
<?php

$envelope["from"]= "joe@example.com";
$envelope["to"]  = "foo@example.com";
$envelope["cc"]  = "bar@example.com";

$part1["type"] = TYPEMULTIPART;
$part1["subtype"] = "mixed";

$part2["type"] = TYPEAPPLICATION;
$part2["encoding"] = ENCBINARY;
$part2["subtype"] = "octet-stream";
$part2["description"] = "some file";
$part2["contents.data"] = "ABC";

$part3["type"] = TYPETEXT;
$part3["subtype"] = "plain";
$part3["description"] = "description3";
$part3["contents.data"] = "contents.data3\n\n\n\t";

$body[1] = $part1;
$body[2] = $part2;
$body[3] = $part3;

echo imap_mail_compose($envelope, $body);

?>
--EXPECTF--
From: joe@example.com
To: foo@example.com
cc: bar@example.com
MIME-Version: 1.0
Content-Type: MULTIPART/mixed; BOUNDARY="%s"

--%s
Content-Type: APPLICATION/octet-stream
Content-Transfer-Encoding: BASE64
Content-Description: some file

QUJD

--%s
Content-Type: TEXT/plain; CHARSET=US-ASCII
Content-Description: description3

contents.data3


	
--%s--
