--TEST--
Bug #42859 import always conflicts with internal classes
--FILE--
<?php
namespace Foo;
class Ex {}

import Blah::Exception;
import Blah::Ex;
?>
--EXPECTF--
Fatal error: Import name 'Ex' conflicts with defined class in %sbug42859.php on line 6