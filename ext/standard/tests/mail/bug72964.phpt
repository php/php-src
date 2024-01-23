--TEST--
Bug #72964 (White space not unfolded for CC/Bcc headers)
--SKIPIF--
<?php
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
require_once __DIR__.'/mail_windows_skipif.inc';
?>
--INI--
SMTP=localhost
smtp_port=1025
sendmail_from=from@example.com
--FILE--
<?php

require_once __DIR__.'/mailpit_utils.inc';

$to = 'bug72964_to@example.com';
$from = ini_get('sendmail_from');
$cc = ['bug72964_cc_1@example.com', 'bug72964_cc_2@example.com'];
$bcc = ['bug72964_bcc_1@example.com', 'bug72964_bcc_2@example.com'];
$subject = bin2hex(random_bytes(16));
$message = 'hello';
$headers = "From: {$from}\r\n"
    . "Cc: {$cc[0]},\r\n\t{$cc[1]}\r\n"
    . "Bcc: {$bcc[0]},\r\n {$bcc[1]}\r\n";

$res = mail($to, $subject, $message, $headers);

if ($res !== true) {
    exit("Unable to send the email.\n");
}

echo "Email sent.\n";

$res = searchEmailByToAddress($to);

if (mailCheckResponse($res, $from, $to, $subject, $message)) {
    echo "Found the email sent.\n";

    $ccAddresses = getCcAddresses($res);
    if (in_array($cc[0], $ccAddresses, true)) {
        echo "cc1 is set.\n";
    }

    if (in_array($cc[1], $ccAddresses, true)) {
        echo "cc2 is set.\n";
    }

    $bccAddresses = getBccAddresses($res);
    if (in_array($bcc[0], $bccAddresses, true)) {
        echo "bcc1 is set.\n";
    }

    if (in_array($bcc[1], $bccAddresses, true)) {
        echo "bcc2 is set.";
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
cc1 is set.
cc2 is set.
bcc1 is set.
bcc2 is set.
