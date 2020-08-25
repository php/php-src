--TEST--
SoapFault class: Invalid Fault code warning given? Can't be an empty string, an array of not 2 elements etc.
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$fault = new SoapFault("", "message"); // Can't be an empty string

try {
    new SoapFault(new stdClass(), "message");  // Can't be a non-string (except for null)
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

$fault = new SoapFault("Sender", "message");
echo get_class($fault) . "\n";
$fault = new SoapFault(null, "message");
echo get_class($fault) . "\n";
$fault = new SoapFault(["more"], "message");  // two elements in array required
$fault = new SoapFault(["m", "more", "superfluous"], "message"); // two required
$fault = new SoapFault(["more-ns", "Sender"], "message");  // two given
echo get_class($fault);
?>
--EXPECTF--
Warning: SoapFault::__construct(): Invalid fault code in %s on line %d
SoapFault::__construct(): Argument #1 ($faultcode) must be of type string|array|null, stdClass given
SoapFault
SoapFault

Warning: SoapFault::__construct(): Invalid fault code in %s on line %d

Warning: SoapFault::__construct(): Invalid fault code in %s on line %d
SoapFault
