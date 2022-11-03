--TEST--
GH-7875 (mails are sent even if failure to log throws exception)
--INI--
sendmail_path={MAIL:{PWD}/gh7875.mail.out}
mail.log={PWD}/gh7875.mail.log
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== "Windows") {
    if (!extension_loaded('posix')) die('skip POSIX extension not loaded');
    if (posix_geteuid() == 0) die('skip Cannot run test as root.');
}
?>
--FILE--
<?php
function exception_error_handler($severity, $message, $file, $line) {
    if (!(error_reporting() & $severity)) {
        return;
    }
    throw new ErrorException($message, 0, $severity, $file, $line);
}
set_error_handler("exception_error_handler");

touch(__DIR__ . "/gh7875.mail.log");
chmod(__DIR__ . "/gh7875.mail.log", 0444);

try {
	mail('recipient@example.com', 'Subject', 'Body', []);
	echo 'Not Reached';
} catch (\Exception $e) {
	echo $e->getMessage(), PHP_EOL;
    var_dump(file_exists(__DIR__ . "/gh7875.mail.out"));
}
?>
--CLEAN--
<?php
@chmod(__DIR__ . "/gh7875.mail.log", 0644);
@unlink(__DIR__ . "/gh7875.mail.log");
@unlink(__DIR__ . "/gh7875.mail.out");
?>
--EXPECTF--
mail(%s): Failed to open stream: Permission denied
bool(false)
