--TEST--
Bug #72964 (White space not unfolded for CC/Bcc headers)
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
$cc = ['cc1' => $users[0], 'cc2' => $users[1]];
$bcc = ['bcc1' => $users[2], 'bcc2' => $users[3]];
$subject = 'mail_bug72964';
$message = 'hello';
$headers = "From: {$from}\r\n"
    . "Cc: {$cc['cc1']},\r\n\t{$cc['cc2']}\r\n"
    . "Bcc: {$bcc['bcc1']},\r\n {$bcc['bcc2']}\r\n";

$res = mail($to, $subject, $message, $headers);

if ($res !== true) {
    die("Unable to send the email.\n");
}

echo "Email sent.\n";

foreach ([['to' => $to], $cc, $bcc] as $mailAddresses) {
    foreach ($mailAddresses as $recipient => $mailAddress) {
        $mailBox = MailBox::login($mailAddress);
        $mail = $mailBox->getMailsBySubject($subject);
        $mailBox->logout();

        if ($mail->isAsExpected($from, $to, $subject, $message)) {
            echo "Found the email. {$recipient} received.\n";
        }
    }
}
?>
--CLEAN--
<?php
require_once __DIR__.'/mail_util.inc';
$subject = 'mail_bug72964';
foreach (MailBox::USERS as $mailAddress) {
    $mailBox = MailBox::login($mailAddress);
    $mailBox->deleteMailsBySubject($subject);
    $mailBox->logout();
}
?>
--EXPECT--
Email sent.
Found the email. to received.
Found the email. cc1 received.
Found the email. cc2 received.
Found the email. bcc1 received.
Found the email. bcc2 received.
