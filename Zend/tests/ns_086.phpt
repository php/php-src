--TEST--
086: bracketed namespace with encoding
--SKIPIF--
<?php
if (!extension_loaded("mbstring")) {
  die("skip Requires mbstring extension");
}
?>
--INI--
zend.multibyte=1
--FILE--
<?php
declare(encoding='utf-8');
namespace foo {
use \foo;
class bar {
    function __construct() {echo __METHOD__,"\n";}
}
new foo;
new bar;
}
namespace {
class foo {
    function __construct() {echo __METHOD__,"\n";}
}
use foo\bar as foo1;
new foo1;
new foo;
echo "===DONE===\n";
}
?>
--EXPECT--
foo::__construct
foo\bar::__construct
foo\bar::__construct
foo::__construct
===DONE===
