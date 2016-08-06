--TEST--
Test mail() function : array extra header basic functionality
--INI--
sendmail_path=tee mailBasic.out >/dev/null
mail.add_x_header = Off
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip Won't run on Windows");
?>
--FILE--
<?php
/* Prototype  : int mail(string to, string subject, string message [, string additional_headers [, string additional_parameters]])
 * Description: Send an email message
 * Source code: ext/standard/mail.c
 * Alias to functions:
 */

error_reporting(-1);

echo "*** Testing mail() : basic functionality ***\n";

echo "\n\n************* TEST ******************\n";
// Should pass
// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
$additional_headers = array(
	'KHeaders' => 'aaaa',
	'bcc'=>'foo@bar',
	'foo'=>
	array(
		"bar\r\n hoge",
		"bar\r\n\t fuga",
	),
);
$outFile = "mailBasic.out";
@unlink($outFile);

echo "-- All Mail Content Parameters --\n";
// Calling mail() with all additional headers
var_dump( mail($to, $subject, $message, $additional_headers) );
echo file_get_contents($outFile);
unlink($outFile);

echo "\n-- Mandatory Parameters --\n";
// Calling mail() with mandatory arguments
var_dump( mail($to, $subject, $message) );
echo file_get_contents($outFile);
unlink($outFile);


echo "\n\n************* TEST ******************\n";
// Should fail all
// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
// Headers should not have array values
$additional_headers = array(
	'orig-date' => array('foo1'),
	'from' => array('foo2'),
	'sender' => array('foo3'),
	'reply-to' => array('foo4'),
	'to' => array('foo5'),
	'bcc' => array('foo6'),
	'message-id' => array('foo7'),
	'in-reply-to'=> array('foo8'),
);
$outFile = "mailBasic.out";
@unlink($outFile);

echo "-- All Mail Content Parameters --\n";
// Calling mail() with all additional headers
var_dump( mail($to, $subject, $message, $additional_headers) );
echo file_get_contents($outFile);
unlink($outFile);

echo "\n-- Mandatory Parameters --\n";
// Calling mail() with mandatory arguments
var_dump( mail($to, $subject, $message) );
echo file_get_contents($outFile);
unlink($outFile);


echo "\n\n************* TEST ******************\n";
// Should fail all
// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
$additional_headers = array(
	'foo1' => array('foo1'=>'bar1'),
	'foo2' => array('foo2', array('foo3')),
	'foo3' => array(123),
	'foo4' => array(123.456),
	'foo5' => array(FALSE),
	'foo6' => array(NULL),
	'foo7' => array(new StdClass),
);
$outFile = "mailBasic.out";
@unlink($outFile);

echo "-- All Mail Content Parameters --\n";
// Calling mail() with all additional headers
var_dump( mail($to, $subject, $message, $additional_headers) );
echo file_get_contents($outFile);
unlink($outFile);

echo "\n-- Mandatory Parameters --\n";
// Calling mail() with mandatory arguments
var_dump( mail($to, $subject, $message) );
echo file_get_contents($outFile);
unlink($outFile);

?>
===DONE===
--EXPECTF--
*** Testing mail() : basic functionality ***


************* TEST ******************
-- All Mail Content Parameters --
bool(true)
To: user@example.com
Subject: Test Subject
KHeaders: aaaa
bcc: foo@bar
foo: bar
 hoge
foo: bar
	 fuga

A Message

-- Mandatory Parameters --
bool(true)
To: user@example.com
Subject: Test Subject

A Message


************* TEST ******************
-- All Mail Content Parameters --

Warning: mail(): 'orig-date' header must be at most one header. Array is passed for 'orig-date' in %s on line 65

Warning: mail(): 'from' header must be at most one header. Array is passed for 'from' in %s on line 65

Warning: mail(): 'sender' header must be at most one header. Array is passed for 'sender' in %s on line 65

Warning: mail(): 'reply-to' header must be at most one header. Array is passed for 'reply-to' in %s on line 65

Warning: mail(): Extra header cannot contain 'To' header in %s on line 65

Warning: mail(): 'bcc' header must be at most one header. Array is passed for 'bcc' in %s on line 65

Warning: mail(): 'message-id' header must be at most one header. Array is passed for 'message-id' in %s on line 65

Warning: mail(): 'in-reply-to' header must be at most one header. Array is passed for 'in-reply-to' in %s on line 65
bool(true)
To: user@example.com
Subject: Test Subject

A Message

-- Mandatory Parameters --
bool(true)
To: user@example.com
Subject: Test Subject

A Message


************* TEST ******************
-- All Mail Content Parameters --

Warning: mail(): Multiple header key must be numeric index (foo1) in %s on line 96

Warning: mail(): Multiple header values must be string (foo2) in %s on line 96

Warning: mail(): Multiple header values must be string (foo3) in %s on line 96

Warning: mail(): Multiple header values must be string (foo4) in %s on line 96

Warning: mail(): Multiple header values must be string (foo5) in %s on line 96

Warning: mail(): Multiple header values must be string (foo6) in %s on line 96

Warning: mail(): Multiple header values must be string (foo7) in %s on line 96
bool(true)
To: user@example.com
Subject: Test Subject
foo2: foo2

A Message

-- Mandatory Parameters --
bool(true)
To: user@example.com
Subject: Test Subject

A Message
===DONE===
