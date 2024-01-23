--TEST--
Bug #80751 (Comma in recipient name breaks email delivery)
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

require_once __DIR__.'/mailpit_utils.inc';

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
}

echo "Email sent.\n";

$res = searchEmailByToAddress($to);

if (mailCheckResponse($res, $from, $to, $subject, $message)) {
    echo "Found the email sent.\n";

    $ccAddresses = getCcAddresses($res);
    if (in_array($cc, $ccAddresses, true)) {
        echo "cc is set.\n";
    }

    $bccAddresses = getBccAddresses($res);
    if (in_array($bcc, $bccAddresses, true)) {
        echo "bcc is set.\n";
    }

    if ($res['ReturnPath'] === $from) {
        echo "Return-Path is as expected.\n";
    }

    $headers = getHeaders($res);
    if ($headers['To'][0] === $toLine) {
        echo "To header is as expected.\n";
    }

    if ($headers['From'][0] === $fromLine) {
        echo "From header is as expected.\n";
    }

    if ($headers['Cc'][0] === $ccLine) {
        echo "Cc header is as expected.";
    }
}
?>
--CLEAN--
<?php
require_once __DIR__.'/mailpit_utils.inc';
deleteEmailByToAddress('bug72964_to@example.com');
?>
--EXPECT--
Email sent.
Found the email sent.
cc is set.
bcc is set.
Return-Path is as expected.
To header is as expected.
From header is as expected.
Cc header is as expected.
