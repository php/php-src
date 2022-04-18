--TEST--
mb_send_mail() test 2 (lang=Japanese)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_send_mail") || !mb_language("japanese")) {
    die("skip mb_send_mail() not available");
}
?>
--INI--
sendmail_path={MAIL:{PWD}/mb_send_mail02.eml}
mail.add_x_header=off
--FILE--
<?php
$to = 'example@example.com';

/* default setting */
mb_send_mail($to, mb_language(), "test");
readfile(__DIR__ . "/mb_send_mail02.eml");

/* Japanese (EUC-JP) */
if (mb_language("japanese")) {
    mb_internal_encoding('EUC-JP');
    mb_send_mail($to, "¥Æ¥¹¥È ".mb_language(), "¥Æ¥¹¥È");
    readfile(__DIR__ . "/mb_send_mail02.eml");
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/mb_send_mail02.eml");
?>
--EXPECTF--
To: example@example.com
Subject: %s
MIME-Version: 1.0
Content-Type: text/plain; charset=%s
Content-Transfer-Encoding: %s

%s
To: example@example.com
Subject: =?ISO-2022-JP?B?GyRCJUYlOSVIGyhCIEphcGFuZXNl?=
MIME-Version: 1.0
Content-Type: text/plain; charset=ISO-2022-JP
Content-Transfer-Encoding: 7bit

$B%F%9%H(B
