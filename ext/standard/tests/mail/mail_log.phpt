--TEST--
Test mail() function : mail.log ini setting
--INI--
sendmail_path={MAIL:mail.out}
mail.log = mail.log
--FILE--
<?php
date_default_timezone_set("UTC");

$logfile = ini_get("mail.log");
if (file_exists($logfile)) {
    unlink($logfile);
}
touch($logfile);
clearstatcache();

$to = "test@example.com";
$subject = "mail.log test";
$message = "Testing mail.log";
$headers = "X-Test: 1";

var_dump(filesize($logfile) == 0);
clearstatcache();

var_dump(mail($to, $subject, $message, $headers));

var_dump(filesize($logfile) > 0);
clearstatcache();

echo file_get_contents($logfile);
?>
Done
--CLEAN--
<?php
unlink("mail.log");
unlink("mail.out");
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
[%d-%s-%d %d:%d:%d UTC] mail() on [%smail_log.php:%d]: To: test@example.com -- Headers: X-Test: 1 -- Subject: mail.log test
Done
