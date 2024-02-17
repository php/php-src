--TEST--
GH-13415 (Added validation of line breaks \n in $additional_headers of mail())
--INI--
sendmail_path={MAIL:gh13415.out}
--FILE--
<?php
mail('to@example.com', 'Test Subject', 'A Message', ['Reply-To' => "foo@example.com \nCc: hacker@example.com"]);
?>
--CLEAN--
<?php
if (file_exists('gh13415.out')) {
    unlink('gh13415.out');
}
?>
--EXPECTF--
Fatal error: Uncaught ValueError: Header "Reply-To" has invalid format, or contains invalid characters in %s
Stack trace:
#0 %s: mail('to@example.com', 'Test Subject', 'A Message', Array)
#1 {main}
  thrown in %s
