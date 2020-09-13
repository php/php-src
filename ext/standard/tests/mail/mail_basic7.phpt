--TEST--
Test mail() function : array extra header basic functionality
--INI--
sendmail_path={MAIL:mailBasic7.out}
mail.add_x_header = Off
--FILE--
<?php
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
$outFile = "mailBasic7.out";
@unlink($outFile);

echo "-- All Mail Content Parameters --\n";
// Calling mail() with all additional headers
try {
    var_dump(mail($to, $subject, $message, $additional_headers));
    echo file_get_contents($outFile);
    unlink($outFile);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

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
$outFile = "mailBasic7.out";
@unlink($outFile);

echo "-- All Mail Content Parameters --\n";
// Calling mail() with all additional headers
try {
    var_dump(mail($to, $subject, $message, $additional_headers));
    echo file_get_contents($outFile);
    unlink($outFile);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

echo "\n\n************* TEST ******************\n";
// Should fail all
// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
$additional_headers = array(
    'foo1' => array('foo2'=>'bar1'),
    'foo2' => array('foo2', array('foo3')),
    'foo3' => array(123),
    'foo4' => array(123.456),
    'foo5' => array(FALSE),
    'foo6' => array(NULL),
    'foo7' => array(new StdClass),
);
$outFile = "mailBasic7.out";
@unlink($outFile);

echo "-- All Mail Content Parameters --\n";
// Calling mail() with all additional headers
try {
    var_dump(mail($to, $subject, $message, $additional_headers));
    echo file_get_contents($outFile);
    unlink($outFile);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

echo "\n\n************* TEST ******************\n";
// Should fail most
// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';
$additional_headers = array(
    '*:foo1' => array('bar1'),
    'foo2:::' => array('bar1'),
    'foo3()' => array('bar1'),
    'foo4@' => array('bar1'),
    'foo5|' => array('bar1'),
    "\0foo6" => array('bar1'),
    "foo7\0" => array('bar1'),
    "foo8" => array(),
    "foo9" => '%&$#!',
    "foo10" => "abc\0\tdef",
);
$outFile = "mailBasic7.out";
@unlink($outFile);

echo "-- All Mail Content Parameters --\n";
// Calling mail() with all additional headers
try {
    var_dump(mail($to, $subject, $message, $additional_headers));
    echo file_get_contents($outFile);
    unlink($outFile);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

?>
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


************* TEST ******************
-- All Mail Content Parameters --
TypeError: Header "orig-date" must be of type string, array given


************* TEST ******************
-- All Mail Content Parameters --
TypeError: Header "foo1" must only contain numeric keys, "foo2" found


************* TEST ******************
-- All Mail Content Parameters --
ValueError: Header name "*:foo1" contains invalid characters
