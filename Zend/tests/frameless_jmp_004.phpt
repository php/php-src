--TEST--
Frameless jmp
--FILE--
<?php
namespace Foo;
function declare_local_class_exists() {
    function class_exists() {
        var_dump(__FUNCTION__);
        return true;
    }
}
var_dump(CLASS_EXISTS('Foo'));
declare_local_class_exists();
var_dump(CLASS_EXISTS('Foo'));
?>
--EXPECT--
bool(false)
string(16) "Foo\class_exists"
bool(true)
