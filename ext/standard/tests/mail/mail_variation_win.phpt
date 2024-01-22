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

$from = ini_get('sendmail_from');

$cases = [
    [
        'label' => 'From is not set',
        'premise' => function () {
            ini_set('SMTP', 'localhost');
            ini_set('smtp_port', 1025);
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
            ini_set('smtp_port', 1025);
            ini_set('sendmail_from', 'from@example.com');
        }
    ],
];

foreach ($cases as $index => ['label' => $label, 'premise' => $premise]) {
    echo "========== {$label} ==========\n";

    $premise();

    $to = "mail_variation_win_{$index}@example.com";
    $subject = "{$index}: Basic PHPT test for mail() function";
    $message = <<<HERE
Description
bool mail ( string \$to , string \$subject , string \$message [, string \$additional_headers [, string \$additional_parameters]] )
Send an email message
HERE;

    $res = mail($to, $subject, $message);

    if ($res === true) {
        echo "Sent the email.\n";
    }

    $res = searchEmailByToAddress($to);

    if (mailCheckResponse($res, $from, $to, $subject, $message)) {
        echo "Received the email.\n\n";
        deleteEmail($res);
    }
}
?>
--EXPECTF--
========== From is not set ==========

Warning: mail(): Bad Message Return Path in %s on line %d
========== Invalid port ==========

Warning: mail(): Failed to connect to mailserver at "localhost" port 2525, verify your "SMTP" and "smtp_port" setting in php.ini or use ini_set() in %s on line %d
========== Invalid host ==========

Warning: mail(): Failed to connect to mailserver at "localplace" port 1025, verify your "SMTP" and "smtp_port" setting in php.ini or use ini_set() in %s on line %d

