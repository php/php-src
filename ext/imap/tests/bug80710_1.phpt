--TEST--
Bug #80710 (imap_mail_compose() header injection) - MIME Splitting Attack
--SKIPIF--
<?php
if (!extension_loaded("imap")) die("skip imap extension not available");
?>
--FILE--
<?php
$envelope["from"]= "joe@example.com\n From : X-INJECTED";
$envelope["to"]  = "foo@example.com\nFrom: X-INJECTED";
$envelope["cc"]  = "bar@example.com\nFrom: X-INJECTED";
$envelope["subject"]  = "bar@example.com\n\n From : X-INJECTED";
$envelope["x-remail"]  = "bar@example.com\nFrom: X-INJECTED";
$envelope["something"]  = "bar@example.com\nFrom: X-INJECTED";

$part1["type"] = TYPEMULTIPART;
$part1["subtype"] = "mixed";

$part2["type"] = TYPEAPPLICATION;
$part2["encoding"] = ENCBINARY;
$part2["subtype"] = "octet-stream\nContent-Type: X-INJECTED";
$part2["description"] = "some file\nContent-Type: X-INJECTED";
$part2["contents.data"] = "ABC\nContent-Type: X-INJECTED";

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
Warning: imap_mail_compose(): header injection attempt in from in %s on line %d
