--TEST--
SoapFault class: Invalid Fault code warning given? Can't be an empty string, an array of not 2 elements etc.
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

try {
    new SoapFault("", "message"); // Can't be an empty string
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    new SoapFault(new stdClass(), "message");  // Can't be a non-string (except for null)
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

$fault = new SoapFault("Sender", "message");
echo get_class($fault) . "\n";
$fault = new SoapFault(null, "message");
echo get_class($fault) . "\n";

try {
    new SoapFault(["more"], "message");  // two elements in array required
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    new SoapFault(["m", "more", "superfluous"], "message"); // two required
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

$fault = new SoapFault(["more-ns", "Sender"], "message");  // two given
echo get_class($fault);

?>
--EXPECT--
SoapFault::__construct(): Argument #1 ($code) is not a valid fault code
SoapFault::__construct(): Argument #1 ($code) must be of type array|string|null, stdClass given
SoapFault
SoapFault
SoapFault::__construct(): Argument #1 ($code) is not a valid fault code
SoapFault::__construct(): Argument #1 ($code) is not a valid fault code
SoapFault
