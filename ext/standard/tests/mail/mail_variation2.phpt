--TEST--
Test mail() function : variation force extra parameters
--INI--
sendmail_path="cat > /tmp/php_test_mailVariation2.out"
mail.force_extra_parameters="-n"
mail.add_x_header = Off
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
?>
--FILE--
<?php
echo "*** Testing mail() : basic functionality ***\n";


// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
$outFile = "/tmp/php_test_mailVariation2.out";
@unlink($outFile);

var_dump( mail($to, $subject, $message) );
echo file_get_contents($outFile);
unlink($outFile);

?>
--EXPECTF--
*** Testing mail() : basic functionality ***
bool(true)
%w1%wTo: user@example.com
%w2%wSubject: Test Subject
%w3%w
%w4%wA Message
