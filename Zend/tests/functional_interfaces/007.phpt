--TEST--
functional interfaces: cannot implement self
--FILE--
<?php
function () implements self {};
--EXPECTF--
Fatal error: functional interface cannot implement self in %s on line 2
