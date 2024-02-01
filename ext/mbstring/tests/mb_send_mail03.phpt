--TEST--
mb_send_mail() test 3 (lang=English)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_send_mail") || !mb_language("english")) {
    die("skip mb_send_mail() not available");
}
?>
--INI--
sendmail_path={MAIL:{PWD}/mb_send_mail03.eml}
mail.add_x_header=off
--FILE--
<?php
$to = 'example@example.com';

/* default setting */
mb_send_mail($to, mb_language(), "test");
readfile(__DIR__ . "/mb_send_mail03.eml");

/* English (iso-8859-1) */
if (mb_language("english")) {
    mb_internal_encoding("ISO-8859-1");
    mb_send_mail($to, "test ".mb_language(), "test");
    readfile(__DIR__ . "/mb_send_mail03.eml");
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/mb_send_mail03.eml");
?>
--EXPECTF--
To: example@example.com
Subject: %s
MIME-Version: 1.0
Content-Type: text/plain; charset=%s
Content-Transfer-Encoding: %s

%s
To: example@example.com
Subject: test English
MIME-Version: 1.0
Content-Type: text/plain; charset=%s-8859-1
Content-Transfer-Encoding: 8bit

test
