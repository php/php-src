--TEST--
mb_send_mail() test 4 (lang=German)
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

/* German (iso-8859-15) */
if (mb_language("german")) {
	mb_internal_encoding("ISO-8859-15");
	mb_send_mail($to, "Pr"."\xfc"."fung ".mb_language(), "Pr"."\xfc"."fung");
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
Subject: =?ISO-8859-15?Q?Pr=FCfung=20German?=
Mime-Version: 1.0
Content-Type: text/plain; charset=%s-8859-15
Content-Transfer-Encoding: 8bit

Prüfung
