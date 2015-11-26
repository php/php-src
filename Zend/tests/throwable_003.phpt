--TEST--
Test user code implementing Throwable results in fatal error
--CREDITS--
Aaron Piotrowski <aaron@icicle.io>
--FILE--
<?php
class Failure implements Throwable {}
?>
--EXPECTF--

Fatal error: Class Failure cannot implement interface Throwable, extend Exception or Error instead in %s on line %d
