--TEST--
Bug #52681 (mb_send_mail() appends an extra MIME-Version header)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists("mb_send_mail") || !mb_language("neutral")) {
    die("skip mb_send_mail() not available");
}
?>
--INI--
sendmail_path={MAIL:{PWD}/bug52681.eml}
mail.add_x_header=off
--FILE--
<?php
$to = 'example@example.com';
$headers = 'MIME-Version: 2.0';

mb_send_mail($to, mb_language(), "test", $headers);

readfile(__DIR__ . "/bug52681.eml");
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug52681.eml");
?>
--EXPECTF--
To: example@example.com
Subject: %s
MIME-Version: 2.0
Content-Type: text/plain; charset=%s
Content-Transfer-Encoding: %s

%s
