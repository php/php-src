--TEST--
mb_send_mail() test 1 (lang=neutral)
--SKIPIF--
<?php
if (@mb_send_mail() === false || !mb_language("neutral")) {
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

/* neutral (UTF-8) */
if (mb_language("neutral")) {
	mb_internal_encoding("none");
	mb_send_mail($to, "test ".mb_language(), "test");
}
?>
--EXPECTF--
To: example@example.com
Subject: %s
Mime-Version: 1.0
Content-Type: text/plain; charset=%s
Content-Transfer-Encoding: %s

%s
To: example@example.com
Subject: test neutral
Mime-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: BASE64

dGVzdA==
