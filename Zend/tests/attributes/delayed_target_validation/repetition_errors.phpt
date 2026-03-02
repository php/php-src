--TEST--
#[\DelayedTargetValidation] does not prevent repetition errors
--FILE--
<?php

#[DelayedTargetValidation]
#[NoDiscard]
#[NoDiscard]
class Demo {}

?>
--EXPECTF--
Fatal error: Attribute "NoDiscard" must not be repeated in %s on line %d
