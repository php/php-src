--TEST--
GH-8086 (mb_send_mail() function not working correctly in PHP 8.x)
--EXTENSIONS--
mbstring
--INI--
sendmail_path={MAIL:{PWD}/gh8086.eml}
mail.mixed_lf_and_crlf=on
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

$stream = fopen(__DIR__ . "/gh8086.eml", "rb");
$eml = stream_get_contents($stream);
fclose($stream);
var_dump(preg_match_all('/(?<!\r)\n/', $eml));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh8086.eml");
?>
--EXPECT--
int(6)
