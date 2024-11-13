--TEST--
Test mail() function : basic functionality (win)
--SKIPIF--
<?php die('skip test'); require_once __DIR__.'/mail_windows_skipif.inc'; ?>
--INI--
SMTP=localhost
smtp_port=25
--FILE--
<?php

require_once __DIR__.'/mail_util.inc';
$users = MailBox::USERS;

$from = ini_get('sendmail_from');

$cases = [
    [
        'label' => 'From is not set',
        'premise' => function () {
            ini_set('SMTP', 'localhost');
            ini_set('smtp_port', 25);
        }
    ],
    [
        'label' => 'Invalid port',
        'premise' => function () {
            ini_set('SMTP', 'localhost');
            ini_set('smtp_port', 2525);
            ini_set('sendmail_from', 'from@example.com');
        }
    ],
    [
        'label' => 'Invalid host',
        'premise' => function () {
            ini_set('SMTP', 'localplace');
            ini_set('smtp_port', 25);
            ini_set('sendmail_from', 'from@example.com');
        }
    ],
];

foreach ($cases as $index => ['label' => $label, 'premise' => $premise]) {
    echo "========== {$label} ==========\n";

    $premise();

    $to = $users[$index];
    $subject = "{$index}: Basic PHPT test for mail() function";
    $message = <<<HERE
Description
bool mail ( string \$to , string \$subject , string \$message [, string \$additional_headers [, string \$additional_parameters]] )
Send an email message
HERE;

    $res = mail($to, $subject, $message);

    if ($res === true) {
        echo "Email sent.\n";
    }

    $mailBox = MailBox::login($to);
    $mail = $mailBox->getMailsBySubject($subject);
    $mailBox->logout();

    if ($mail->count() > 0) {
        echo "Found the email.\n\n";
    }
}
?>
--CLEAN--
<?php
require_once __DIR__.'/mail_util.inc';
for ($i = 0; $i <= 2; $i++) {
    $subject = "{$i}: Basic PHPT test for mail() function";
    $mailBox = MailBox::login(MailBox::USERS[$i]);
    $mailBox->deleteMailsBySubject($subject);
    $mailBox->logout();
}
?>
--EXPECTF--
========== From is not set ==========

Warning: mail(): Bad Message Return Path in %s on line %d
========== Invalid port ==========

Warning: mail(): Failed to connect to mailserver at "localhost" port 2525, verify your "SMTP" and "smtp_port" setting in php.ini or use ini_set() in %s on line %d
========== Invalid host ==========

Warning: mail(): Failed to connect to mailserver at "localplace" port 1025, verify your "SMTP" and "smtp_port" setting in php.ini or use ini_set() in %s on line %d

