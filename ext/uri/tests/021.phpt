--TEST--
Test extension of Uri\Rfc3986\Uri
--EXTENSIONS--
uri
--FILE--
<?php

readonly class MyRfc3986Uri extends Uri\Rfc3986\Uri
{
}

?>
--EXPECTF--
Fatal error: Class MyRfc3986Uri cannot extend final class Uri\Rfc3986\Uri in %s on line %d
