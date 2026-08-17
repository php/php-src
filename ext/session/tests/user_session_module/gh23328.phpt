--TEST--
GH-23328: SessionHandlerInterface create_sid()/validateId() warning depends on interface order
--EXTENSIONS--
session
--FILE--
<?php

abstract class HandlerFirst implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface {}
abstract class HandlerLast implements SessionIdInterface, SessionUpdateTimestampHandlerInterface, SessionHandlerInterface {}

interface CombinedInterface extends SessionIdInterface, SessionUpdateTimestampHandlerInterface {}
abstract class CombinedFirst implements CombinedInterface, SessionHandlerInterface {}
abstract class CombinedLast implements SessionHandlerInterface, CombinedInterface {}

interface NestedInterface extends CombinedInterface {}
abstract class NestedLast implements SessionHandlerInterface, NestedInterface {}

abstract class MissingBoth implements SessionHandlerInterface {}
abstract class MissingValidateId implements SessionHandlerInterface, SessionIdInterface {}
abstract class MissingCreateSid implements SessionHandlerInterface, SessionUpdateTimestampHandlerInterface {}

echo "Done\n";
?>
--EXPECTF--
Warning: Class MissingBoth implementing SessionHandlerInterface is missing the create_sid() method which will be required in PHP 9.0 in %s on line %d

Warning: Class MissingBoth implementing SessionHandlerInterface is missing the validateId() method which will be required in PHP 9.0 in %s on line %d

Warning: Class MissingValidateId implementing SessionHandlerInterface is missing the validateId() method which will be required in PHP 9.0 in %s on line %d

Warning: Class MissingCreateSid implementing SessionHandlerInterface is missing the create_sid() method which will be required in PHP 9.0 in %s on line %d
Done
