--TEST--
functional interfaces: cannot implement static
--FILE--
<?php
function () implements static {};
--EXPECTF--
Fatal error: functional interface cannot implement static in %s on line 2
