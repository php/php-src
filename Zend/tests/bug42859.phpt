--TEST--
Bug #42859 (import always conflicts with internal classes)
--FILE--
<?php
namespace Foo;
class Ex {}

use Blah\Exception;
var_dump(new Ex);
?>
--EXPECTF--
object(Foo\Ex)#%d (0) {
}
