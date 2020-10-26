--TEST--
Bug #42859 (import always conflicts with internal classes)
--FILE--
<?php
namespace Foo;
class Ex {}

use Blah\Exception;
use Blah\Ex;
?>
--EXPECTF--
Fatal error: Cannot import class Blah\Ex as Ex, Ex has already been declared in %s on line %d
