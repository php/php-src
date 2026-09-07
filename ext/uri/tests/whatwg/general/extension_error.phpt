--TEST--
Test Uri\WhatWg\Url extension - error
--FILE--
<?php

readonly class MyWhatWgUri extends Uri\WhatWg\Url
{
}

?>
--EXPECTF--
Fatal error: Class MyWhatWgUri cannot extend final class Uri\WhatWg\Url in %s on line %d
