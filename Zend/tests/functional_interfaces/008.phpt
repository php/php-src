--TEST--
functional interfaces: cannot implement parent
--FILE--
<?php
function () implements parent {};
--EXPECTF--
Fatal error: functional interface cannot implement parent in %s on line 2
