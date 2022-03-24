--TEST--
mb_send_mail() test 4 (lang=German)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_send_mail") || !mb_language("german")) {
    die("skip mb_send_mail() not available");
}
?>
--INI--
sendmail_path={MAIL:{PWD}/mb_send_mail04.eml}
mail.add_x_header=off
--FILE--
<?php
$to = 'example@example.com';

/* default setting */
mb_send_mail($to, mb_language(), "test");
readfile(__DIR__ . "/mb_send_mail04.eml");

/* German (iso-8859-15) */
if (mb_language("german")) {
    mb_internal_encoding("ISO-8859-15");
    mb_send_mail($to, "Pr"."\xfc"."fung ".mb_language(), "Pr"."\xfc"."fung");
    readfile(__DIR__ . "/mb_send_mail04.eml");
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/mb_send_mail04.eml");
?>
--EXPECTF--
To: example@example.com
Subject: %s
MIME-Version: 1.0
Content-Type: text/plain; charset=%s
Content-Transfer-Encoding: %s

%s
To: example@example.com
Subject: =?ISO-8859-15?Q?Pr=FCfung=20German?=
MIME-Version: 1.0
Content-Type: text/plain; charset=%s-8859-15
Content-Transfer-Encoding: 8bit

Prüfung
