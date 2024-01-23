--TEST--
Test mail() function : basic functionality (win)
--SKIPIF--
<?php require_once __DIR__.'/mail_windows_skipif.inc'; ?>
--INI--
SMTP=localhost
smtp_port=1025
--FILE--
<?php

require_once __DIR__.'/mailpit_utils.inc';

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

    $to = "mail_basic_win_{$index}@example.com";
    $subject = "{$index}: Basic PHPT test for mail() function";
    $message = <<<HERE
Description
bool mail ( string \$to , string \$subject , string \$message [, string \$additional_headers [, string \$additional_parameters]] )
Send an email message
HERE;

    $res = $premise($from, $to, $subject, $message);

    if ($res !== true) {
        exit("Unable to send the email.\n");
    }

    echo "Email sent.\n";

    $res = searchEmailByToAddress($to);

    if (mailCheckResponse($res, $from, $to, $subject, $message)) {
        echo "Found the email sent.\n\n";
    }
}
?>
--CLEAN--
<?php
require_once __DIR__.'/mailpit_utils.inc';
deleteEmailByToAddress('mail_basic_win_0@example.com');
deleteEmailByToAddress('mail_basic_win_1@example.com');
deleteEmailByToAddress('mail_basic_win_2@example.com');
deleteEmailByToAddress('mail_basic_win_3@example.com');
?>
--EXPECT--
========== Case 0 ==========
Email sent.
Found the email sent.

========== Case 1 ==========
Email sent.
Found the email sent.

========== Case 2 ==========
Email sent.
Found the email sent.

========== Case 3 ==========
Email sent.
Found the email sent.
