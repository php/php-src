--TEST--
mb_send_mail() test 2 (lang=Japanese)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
if (!function_exists("mb_send_mail") || !mb_language("japanese")) {
    die("skip mb_send_mail() not available");
}
?>
--INI--
sendmail_path=/bin/cat
mail.add_x_header=off
--FILE--
<?php
$to = 'example@example.com';

/* default setting */
mb_send_mail($to, mb_language(), "test");

/* Japanese (EUC-JP) */
if (mb_language("japanese")) {
    mb_internal_encoding('EUC-JP');
    mb_send_mail($to, "�ƥ��� ".mb_language(), "�ƥ���");
}
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
