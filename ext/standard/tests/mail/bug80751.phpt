--TEST--
Bug #80751 (Comma in recipient name breaks email delivery)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
require_once __DIR__.'/mail_windows_skipif.inc';
?>
--INI--
SMTP=localhost
smtp_port=25
--FILE--
<?php

require_once __DIR__.'/mail_util.inc';
$users = MailBox::USERS;

$to = $users[0];
$toLine = "\"<bug80751_to_name@example.com>\" <{$to}>";

$from = 'bug80751_from@example.com';
$fromLine = "\"<bug80751_from_name@example.com>\" <{$from}>";

$cc = $users[1];
$ccLine = "\"Lastname, Firstname\\\\\" <{$cc}>";

$bcc = $users[2];
$subject = 'mail_bug80751';
$message = 'hello';

$headers = "From: {$fromLine}\r\n"
    . "Cc: {$ccLine}\r\n"
    . "Bcc: \"Firstname \\\"Ni,ck\\\" Lastname\" <{$bcc}>\r\n";

$res = mail($toLine, $subject, $message, $headers);

if ($res !== true) {
    die("Unable to send the email.\n");
}

echo "Email sent.\n";

foreach (['to' => $to, 'cc' => $cc, 'bcc' => $bcc] as $recipient => $mailAddress) {
    $mailBox = MailBox::login($mailAddress);
    $mail = $mailBox->getMailsBySubject($subject);
    $mailBox->logout();

    if ($mail->isAsExpected($fromLine, $toLine, $subject, $message)) {
        echo "Found the email. {$recipient} received.\n";
    }

    if ($mail->getHeader('Return-Path') === $from) {
        echo "Return-Path is as expected.\n";
    }

    if ($mail->getHeader('Cc') === $ccLine) {
        echo "Cc header is as expected.\n\n";
    }
}
?>
--CLEAN--
<?php
require_once __DIR__.'/mail_util.inc';
$subject = 'mail_bug80751';
foreach ([MailBox::USERS[0], MailBox::USERS[1], MailBox::USERS[2]] as $mailAddress) {
    $mailBox = MailBox::login($mailAddress);
    $mailBox->deleteMailsBySubject($subject);
    $mailBox->logout();
}
?>
--EXPECT--
Email sent.
Found the email. to received.
Return-Path is as expected.
Cc header is as expected.

Found the email. cc received.
Return-Path is as expected.
Cc header is as expected.

Found the email. bcc received.
Return-Path is as expected.
Cc header is as expected.
