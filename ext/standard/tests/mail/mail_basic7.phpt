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

var_dump(mail($to, $subject, $message, $additional_headers));
echo file_get_contents($outFile);
unlink($outFile);

echo "\n\n************* TEST ******************\n";
// Should fail all
// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';

try {
    mail($to, $subject, $message, ['orig-date' => array('foo1')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['from' => array('foo2')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['sender' => array('foo3')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['reply-to' => array('foo4')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['to' => array('foo5')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['bcc' => array('foo6')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['message-id' => array('foo7')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['in-reply-to'=> array('foo8')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

echo "\n\n************* TEST ******************\n";
// Should fail all
// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';

try {
    mail($to, $subject, $message, ['foo1' => array('foo2'=>'bar1')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo2' => array('foo2', array('foo3'))]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo3' => array(123)]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo4' => array(123.456)]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo5' => array(FALSE)]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo6' => array(NULL)]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo7' => array(new StdClass)]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

echo "\n\n************* TEST ******************\n";
// Should fail most
// Initialise all required variables
$to = 'user@example.com';
$subject = 'Test Subject';
$message = 'A Message';

$outFile = "mailBasic7.out";
@unlink($outFile);

try {
    mail($to, $subject, $message, ['*:foo1' => array('bar1')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo2:::' => array('bar1')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo3()' => array('bar1')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo4@' => array('bar1')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ['foo5|' => array('bar1')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ["\0foo6" => array('bar1')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ["foo7\0" => array('bar1')]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

try {
    mail($to, $subject, $message, ["foo8" => array()]);
} catch (TypeError|ValueError $exception) {
    echo get_class($exception) . ": " . $exception->getMessage() . "\n";
}

var_dump(mail($to, $subject, $message, ["foo9" => '%&$#!']));
echo file_get_contents($outFile);
unlink($outFile);

try {
    mail($to, $subject, $message, ["foo10" => "abc\0\tdef"]);
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
TypeError: Header "orig-date" must be of type string, array given
TypeError: Header "from" must be of type string, array given
TypeError: Header "sender" must be of type string, array given
TypeError: Header "reply-to" must be of type string, array given
ValueError: The additional headers cannot contain the "To" header
TypeError: Header "bcc" must be of type string, array given
TypeError: Header "message-id" must be of type string, array given
TypeError: Header "in-reply-to" must be of type string, array given


************* TEST ******************
TypeError: Header "foo1" must only contain numeric keys, "foo2" found
TypeError: Header "foo2" must only contain values of type string, array found
TypeError: Header "foo3" must only contain values of type string, int found
TypeError: Header "foo4" must only contain values of type string, float found
TypeError: Header "foo5" must only contain values of type string, bool found
TypeError: Header "foo6" must only contain values of type string, null found
TypeError: Header "foo7" must only contain values of type string, stdClass found


************* TEST ******************
ValueError: Header name "*:foo1" contains invalid characters
ValueError: Header name "foo2:::" contains invalid characters
ValueError: Header name "" contains invalid characters
ValueError: Header name "foo7" contains invalid characters
bool(true)
To: user@example.com
Subject: Test Subject
foo9: %&$#!

A Message
ValueError: Header "foo10" contains NULL character that is not allowed in the header
