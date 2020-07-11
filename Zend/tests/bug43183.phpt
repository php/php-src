--TEST--
Bug #43183 ("use" of the same class in difference scripts results in a fatal error)
--FILE--
<?php
namespace Test;
use Test\Foo;
class Foo {}
class Bar {}
use Test\Bar;
echo "ok\n";
--EXPECT--
ok
