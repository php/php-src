--TEST--
mb_send_mail() test 3 (lang=English)
--SKIPIF--
<?php
if (@mb_send_mail() === false || !mb_language("german")) {
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

/* English (iso-8859-1) */
if (mb_language("english")) {
	mb_send_mail($to, "test ".mb_language(), "test");
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
Subject: test English
Mime-Version: 1.0
Content-Type: text/plain; charset=%s-8859-1
Content-Transfer-Encoding: 8bit

test
