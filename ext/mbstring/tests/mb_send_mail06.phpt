--TEST--
mb_send_mail() test 6 (lang=Traditional Chinese)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_send_mail") || !mb_language("Traditional Chinese")) {
    die("skip mb_send_mail() not available");
}
if (!@mb_internal_encoding('BIG5')) {
    die("skip BIG5 encoding is not available on this platform");
}
?>
--INI--
sendmail_path={MAIL:{PWD}/mb_send_mail06.eml}
mail.add_x_header=off
--FILE--
<?php
$to = 'example@example.com';

/* default setting */
mb_send_mail($to, mb_language(), "test");
readfile(__DIR__ . "/mb_send_mail06.eml");

/* Traditional Chinese () */
if (mb_language("traditional chinese")) {
    mb_internal_encoding('BIG5');
    mb_send_mail($to, "´úÅç ".mb_language(), "´úÅç");
    readfile(__DIR__ . "/mb_send_mail06.eml");
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/mb_send_mail06.eml");
?>
--EXPECTF--
To: example@example.com
Subject: %s
MIME-Version: 1.0
Content-Type: text/plain; charset=%s
Content-Transfer-Encoding: %s

%s
To: example@example.com
Subject: =?BIG5?B?tPrF5yBUcmFkaXRpb25hbCBDaGluZXNl?=
MIME-Version: 1.0
Content-Type: text/plain; charset=BIG5
Content-Transfer-Encoding: 8bit

´úÅç
