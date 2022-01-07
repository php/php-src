--TEST--
mb_send_mail() test 1 (lang=neutral)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_send_mail") || !mb_language("neutral")) {
    die("skip mb_send_mail() not available");
}
?>
--INI--
sendmail_path={MAIL:{PWD}/mb_send_mail01.eml}
mail.add_x_header=off
--FILE--
<?php
$to = 'example@example.com';

/* default setting */
mb_send_mail($to, mb_language(), "test");
readfile(__DIR__ . "/mb_send_mail01.eml");

/* neutral (UTF-8) */
if (mb_language("neutral")) {
    mb_internal_encoding("UTF-8");
    mb_send_mail($to, "test ".mb_language(), "test");
    readfile(__DIR__ . "/mb_send_mail01.eml");
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/mb_send_mail01.eml");
?>
--EXPECTF--
To: example@example.com
Subject: %s
MIME-Version: 1.0
Content-Type: text/plain; charset=%s
Content-Transfer-Encoding: %s

%s
To: example@example.com
Subject: test neutral
MIME-Version: 1.0
Content-Type: text/plain; charset=UTF-8
Content-Transfer-Encoding: BASE64

dGVzdA==
