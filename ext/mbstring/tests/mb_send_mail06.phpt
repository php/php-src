--TEST--
mb_send_mail() test 6 (lang=Traditional Chinese)
--SKIPIF--
<?php
if (@mb_send_mail() === false || !mb_language("Traditional Chinese")) {
	die("skip mb_send_mail() not available");
}
if (!@mb_internal_encoding('BIG5')) {
	die("skip BIG5 encoding is not avaliable on this platform");
}
?>
--INI--
sendmail_path=cat
--FILE--
<?php
$to = 'example@example.com';

/* default setting */
mb_send_mail($to, mb_language(), "test");

/* Traditional Chinese () */
if (mb_language("traditional chinese")) {
	mb_internal_encoding('BIG5');
	mb_send_mail($to, "´úÅç ".mb_language(), "´úÅç");
}
?>
--EXPECTF--
To: example@example.com
Subject: %s
Mime-Version: 1.0
Content-Type: text/plain; charset=%s
Content-Transfer-Encoding: %s

test
To: example@example.com
Subject: =?BIG5?B?tPrF5yBUcmFkaXRpb25hbCBDaGluZXNl?=
Mime-Version: 1.0
Content-Type: text/plain; charset=BIG5
Content-Transfer-Encoding: 8bit

´úÅç
