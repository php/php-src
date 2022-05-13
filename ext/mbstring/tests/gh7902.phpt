--TEST--
GH-7902 (mb_send_mail may delimit headers with LF only)
--SKIPIF--
<?php
if (!extension_loaded("mbstring")) die("skip mbstring extension not available");
?>
--INI--
sendmail_path={MAIL:{PWD}/gh7902.eml}
--FILE--
<?php
mb_internal_encoding("UTF-8");
mb_language("uni");
$to = "omittedvalidaddress@example.com";
$subject = "test mail";
$message = "body of testing php mail";
$header["Mime-Version"] = "1.0";
$header["Content-Type"] = "text/html; charset=UTF-8";
$header["From"] = "omittedvalidaddress2@example.com";
$header["X-Mailer"] = "PHP/" . phpversion();
mb_send_mail($to, $subject, $message, $header);

$stream = fopen(__DIR__ . "/gh7902.eml", "rb");
$eml = stream_get_contents($stream);
fclose($stream);
var_dump(preg_match_all('/(?<!\r)\n/', $eml));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh7902.eml");
?>
--EXPECT--
int(0)
