--TEST--
Bug #80706 (Headers after Bcc headers may be ignored)
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
require_once __DIR__.'/mail_windows_skipif.inc';
?>
--INI--
SMTP=localhost
smtp_port=1025
sendmail_from=from@example.com
--FILE--
<?php

require_once __DIR__.'/mailhog_utils.inc';

$to = 'bug80706_to@example.com';
$from = ini_get('sendmail_from');
$bcc = 'bug80706_bcc@exsample.com';
$subject = bin2hex(random_bytes(16));
$message = 'hello';
$xMailer = 'bug80706_x_mailer';
$headers = "From: {$from}\r\n"
    . "Bcc: {$bcc}\r\n"
    . "X-Mailer: {$xMailer}";

$res = mail($to, $subject, $message, $headers);

if ($res !== true) {
    exit("Unable to send the email.\n");
} else {
    echo "Sent the email.\n";
}

$res = searchEmailByToAddress($to);

if (
    $res &&
    getFromAddress($res) === $from &&
    getToAddresses($res)[0] === $to &&
    getSubject($res) === $subject &&
    getBody($res) === $message
) {
    echo "Received the email.\n";

    $recipients = getRecipientAddresses($res);

    if (in_array($bcc, $recipients, true)) {
        echo "bcc Received the email.\n";
    }

    $headers = getHeaders($res);
    if ($headers['X-Mailer'][0] === $xMailer) {
        echo "The specified x-Mailer exists.";
    }

    deleteEmail($res);
}
?>
--EXPECT--
Sent the email.
Received the email.
bcc Received the email.
The specified x-Mailer exists.
