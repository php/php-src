--TEST--
GH-13415 (Added validation of line breaks \n in $additional_headers of mail())
--INI--
sendmail_path={MAIL:gh13415.out}
--FILE--
<?php
echo "LF only:\n";
try {
    mail('to@example.com', 'Test Subject', 'A Message', ['Reply-To' => "foo@example.com \nCc: hacker@example.com"]);
} catch (Throwable $e) {
    echo $e->getMessage()."\n\n";
}

echo "CR only:\n";
try {
    mail('to@example.com', 'Test Subject', 'A Message', ['Reply-To' => "foo@example.com \rCc: hacker@example.com"]);
} catch (Throwable $e) {
    echo $e->getMessage()."\n\n";
}

echo "CRLF:\n";
try {
    mail('to@example.com', 'Test Subject', 'A Message', ['Reply-To' => "foo@example.com \r\nCc: hacker@example.com"]);
} catch (Throwable $e) {
    echo $e->getMessage()."\n\n";
}

echo "NULL:\n";
try {
    mail('to@example.com', 'Test Subject', 'A Message', ['Reply-To' => "foo@example.com \0Cc: hacker@example.com"]);
} catch (Throwable $e) {
    echo $e->getMessage()."\n\n";
}
?>
--CLEAN--
<?php
if (file_exists('gh13415.out')) {
    unlink('gh13415.out');
}
?>
--EXPECTF--
LF only:
Header "Reply-To" contains LF character that is not allowed in the header

CR only:
Header "Reply-To" contains CR character that is not allowed in the header

CRLF:
Header "Reply-To" contains CRLF characters that are used as a line separator and are not allowed in the header

NULL:
Header "Reply-To" contains NULL character that is not allowed in the header
