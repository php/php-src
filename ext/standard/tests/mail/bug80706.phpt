--TEST--
Bug #80706 (Headers after Bcc headers may be ignored)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
require_once __DIR__.'/mail_windows_skipif.inc';
?>
--INI--
SMTP=localhost
smtp_port=25
sendmail_from=from@example.com
--FILE--
<?php

require_once __DIR__.'/mail_util.inc';
$users = MailBox::USERS;

$to = $users[0];
$from = ini_get('sendmail_from');
$bcc = $users[2];
$subject = 'mail_bug80706';
$message = 'hello';
$xMailer = 'bug80706_x_mailer';
$headers = "From: {$from}\r\n"
    . "Bcc: {$bcc}\r\n"
    . "X-Mailer: {$xMailer}";

$res = mail($to, $subject, $message, $headers);

if ($res !== true) {
    die("Unable to send the email.\n");
}

echo "Email sent.\n";

foreach (['to' => $to, 'bcc' => $bcc] as $recipient => $mailAddress) {
    $mailBox = MailBox::login($mailAddress);
    $mail = $mailBox->getMailsBySubject($subject);
    $mailBox->logout();

    if ($mail->isAsExpected($from, $to, $subject, $message)) {
        echo "Found the email. {$recipient} received.\n";
    }

    if ($mail->getHeader('X-Mailer') === $xMailer) {
        echo "The specified x-Mailer exists.\n\n";
    }
}
?>
--CLEAN--
<?php
require_once __DIR__.'/mail_util.inc';
$subject = 'mail_bug80706';
foreach ([MailBox::USERS[0], MailBox::USERS[2]] as $mailAddress) {
    $mailBox = MailBox::login($mailAddress);
    $mailBox->deleteMailsBySubject($subject);
    $mailBox->logout();
}
?>
--EXPECT--
Email sent.
Found the email. to received.
The specified x-Mailer exists.

Found the email. bcc received.
The specified x-Mailer exists.
