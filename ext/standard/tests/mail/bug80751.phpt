--TEST--
Bug #80751 (Comma in recipient name breaks email delivery)
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
--FILE--
<?php

require_once __DIR__.'/mailhog_utils.inc';

$to = 'bug80751_to@example.com';
$toLine = "\"<bug80751_to_name@example.com>\" <{$to}>";

$from = 'bug80751_from@example.com';
$fromLine = "\"<bug80751_from_name@example.com>\" <{$from}>";

$cc = 'bug80751_cc@example.com';
$ccLine = "\"Lastname, Firstname\\\\\" <{$cc}>";

$bcc = 'bug80751_bcc@example.com';
$subject = bin2hex(random_bytes(16));
$message = 'hello';

$headers = "From: {$fromLine}\r\n"
    . "Cc: {$ccLine}\r\n"
    . "Bcc: \"Firstname \\\"Ni,ck\\\" Lastname\" <{$bcc}>\r\n";

$res = mail($toLine, $subject, $message, $headers);

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
    getBody($res) === "\r\n".$message // The last newline in the header goes in first.
) {
    echo "Received the email.\n";

    $recipients = getRecipientAddresses($res);

    if (in_array($cc, $recipients, true)) {
        echo "cc Received the email.\n";
    }

    if (in_array($bcc, $recipients, true)) {
        echo "bcc Receive the email.\n";
    }

    $headers = getHeaders($res);
    if ($headers['Return-Path'][0] === "<{$from}>") {
        echo "Return-Path is as expected.\n";
    }

    if ($headers['To'][0] === $toLine) {
        echo "To header is as expected.\n";
    }

    if ($headers['From'][0] === $fromLine) {
        echo "From header is as expected.\n";
    }

    if ($headers['Cc'][0] === $ccLine) {
        echo "Cc header is as expected.";
    }

    deleteEmail($res);
}
?>
--EXPECT--
Sent the email.
Received the email.
cc Received the email.
bcc Receive the email.
Return-Path is as expected.
To header is as expected.
From header is as expected.
Cc header is as expected.
