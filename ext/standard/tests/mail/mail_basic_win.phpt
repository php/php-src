--TEST--
Test mail() function : basic functionality (win)
--SKIPIF--
<?php require_once __DIR__.'/mail_windows_skipif.inc'; ?>
--INI--
SMTP=localhost
smtp_port=25
--FILE--
<?php

require_once __DIR__.'/mail_util.inc';
$users = MailBox::USERS;

$cases = [
    [
        'from' => 'from@example.com',
        'premise' => function ($from, $to, $subject, $message) {
            ini_set('sendmail_from', $from);
            return mail($to, $subject, $message);
        }
    ],
    [
        'from' => 'ex_from@example.com',
        'premise' => function ($from, $to, $subject, $message) {
            ini_restore('sendmail_from');
            $headers = "from: {$from}";
            return mail($to, $subject, $message, $headers);
        }
    ],
    [
        'from' => 'ex_from@example.com',
        'premise' => function ($from, $to, $subject, $message) {
            ini_restore('sendmail_from');
            $headers = "FRom: {$from}";
            return mail($to, $subject, $message, $headers);
        }
    ],
    [
        'from' => 'ex_from@example.com',
        'premise' => function ($from, $to, $subject, $message) {
            ini_restore('sendmail_from');
            $headers = "from: {$from}";
            $parameters = 'addons'; // should be ignored
            return mail($to, $subject, $message, $headers, $parameters);
        }
    ],
];

foreach ($cases as $index => ['from' => $from, 'premise' => $premise]) {
    echo "========== Case {$index} ==========\n";

    $to = $users[$index];
    $subject = "{$index}: Basic PHPT test for mail() function";
    $message = <<<HERE
Description
bool mail ( string \$to , string \$subject , string \$message [, string \$additional_headers [, string \$additional_parameters]] )
Send an email message
HERE;

    $res = $premise($from, $to, $subject, $message);

    if ($res !== true) {
        die("Unable to send the email.\n");
    }

    echo "Email sent.\n";

    $mailBox = MailBox::login($to);
    $mail = $mailBox->getMailsBySubject($subject);
    $mailBox->logout();

    if ($mail->isAsExpected($from, $to, $subject, $message)) {
        echo "Found the email.\n\n";
    }
}
?>
--CLEAN--
<?php
require_once __DIR__.'/mail_util.inc';
for ($i = 0; $i <= 3; $i++) {
    $subject = "{$i}: Basic PHPT test for mail() function";
    $mailBox = MailBox::login(MailBox::USERS[$i]);
    $mailBox->deleteMailsBySubject($subject);
    $mailBox->logout();
}
?>
--EXPECT--
========== Case 0 ==========
Email sent.
Found the email.

========== Case 1 ==========
Email sent.
Found the email.

========== Case 2 ==========
Email sent.
Found the email.

========== Case 3 ==========
Email sent.
Found the email.
