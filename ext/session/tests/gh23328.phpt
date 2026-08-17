--TEST--
GH-23328: SessionHandlerInterface create_sid()/validateId() warning depends on the order interfaces are listed in
--EXTENSIONS--
session
--FILE--
<?php

abstract class HandlerFirst implements SessionHandlerInterface, SessionIdInterface, SessionUpdateTimestampHandlerInterface {}
abstract class HandlerLast implements SessionIdInterface, SessionUpdateTimestampHandlerInterface, SessionHandlerInterface {}

interface CombinedInterface extends SessionIdInterface, SessionUpdateTimestampHandlerInterface {}
abstract class HandlerCombined implements SessionHandlerInterface, CombinedInterface {}

abstract class HandlerAlone implements SessionHandlerInterface {}

echo "Done\n";
?>
--EXPECTF--
Warning: Class HandlerAlone implementing SessionHandlerInterface is missing the create_sid() method which will be required in PHP 9.0 in %s on line %d

Warning: Class HandlerAlone implementing SessionHandlerInterface is missing the validateId() method which will be required in PHP 9.0 in %s on line %d
Done
