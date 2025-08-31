--TEST--
ASSIGN_OBJ on null reference returned from __get()
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
class Test {
    public $prop;
    public function &__get($name) {
        return $this->prop;
    }
}
function test() {
    $obj = new Test;
    $obj->x->y = 1;
}
function test2() {
    $obj = new Test;
    $obj->x->y += 1;
}
try {
    test();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    test2();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Attempt to assign property "y" on null
Attempt to assign property "y" on null
