--TEST--
Bug #52681 (mb_send_mail() appends an extra MIME-Version header)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not valid for Windows');
}
if (!function_exists("mb_send_mail") || !mb_language("neutral")) {
    die("skip mb_send_mail() not available");
}
?>
--INI--
sendmail_path=/bin/cat
mail.add_x_header=off
--FILE--
<?php
$to = 'example@example.com';
$headers = 'MIME-Version: 2.0';

mb_send_mail($to, mb_language(), "test", $headers);
?>
--EXPECTF--
To: example@example.com
Subject: %s
MIME-Version: 2.0
Content-Type: text/plain; charset=%s
Content-Transfer-Encoding: %s

%s
