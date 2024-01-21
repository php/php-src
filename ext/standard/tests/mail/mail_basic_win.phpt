--TEST--
Test mail() function : basic functionality (win)
--EXTENSIONS--
curl
--SKIPIF--
<?php require_once __DIR__.'/mail_windows_skipif.inc'; ?>
--INI--
SMTP=localhost
smtp_port=1025
--FILE--
<?php

require_once __DIR__.'/mailhog_utils.inc';

$cases = [
    [
        'from' => 'from@example.com',
        'premise' => function ($from, $to, $subject, $message) {
            ini_set('sendmail_from', $from);
            return mail($to, $subject, $message);
        }
    ],
    [
        'from' => 'from@example.com',
        'premise' => function ($from, $to, $subject, $message) {
            ini_restore('sendmail_from');
            $headers = "from: {$from}";
            return mail($to, $subject, $message, $headers);
        }
    ],
    [
        'from' => 'from@example.com',
        'premise' => function ($from, $to, $subject, $message) {
            ini_restore('sendmail_from');
            $headers = "FRom: {$from}";
            return mail($to, $subject, $message, $headers);
        }
    ],
    [
        'from' => 'from@example.com',
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
        echo "Received the email.\n\n";
        deleteEmail($res);
    }
}
?>
--EXPECT--
========== Case 0 ==========
Sent the email.
Received the email.

========== Case 1 ==========
Sent the email.
Received the email.

========== Case 2 ==========
Sent the email.
Received the email.

========== Case 3 ==========
Sent the email.
Received the email.
