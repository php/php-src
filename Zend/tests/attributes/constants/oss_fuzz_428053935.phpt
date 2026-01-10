--TEST--
OSS-Fuzz #428053935
--FILE--
<?php
namespace Foo; // Capital letter is important for the ns lookup
#[Attr]
const MyConst = '';

$rc = new \ReflectionConstant('Foo\\MyConst');
var_dump($rc->getAttributes());
?>
--EXPECTF--
array(1) {
  [0]=>
  object(ReflectionAttribute)#%d (1) {
    ["name"]=>
    string(8) "Foo\Attr"
  }
}
