--TEST--
mb_send_mail() test 5 (lang=Simplified Chinese)
--SKIPIF--
<?php
if (@mb_send_mail() === false || !mb_language("Simplified Chinese")) {
	die("skip mb_send_mail() not available");
}
?>
--INI--
sendmail_path=cat
--FILE--
<?php
$to = 'example@example.com';

/* default setting */
mb_send_mail($to, mb_language(), "test");

/* Simplified Chinese (HK-GB-2312) */
if (mb_language("simplified chinese")) {
	mb_internal_encoding('GB2312');
	mb_send_mail($to, "Втбщ ".mb_language(), "Втбщ");
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
Subject: =?HZ-GB-2312?B?fnsyYlFpfn0gU2ltcGxpZmllZCBD?=
 =?HZ-GB-2312?B?aGluZXNl?=
Mime-Version: 1.0
Content-Type: text/plain; charset=HZ-GB-2312
Content-Transfer-Encoding: 7bit

~{2bQi~}
