--TEST--
GH-24337 (JIT: segfault in zend_get_executed_lineno when decrementing a non-numeric string property)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=tracing
opcache.jit_hot_loop=2
opcache.jit_hot_func=2
opcache.jit_hot_return=2
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--FILE--
<?php
set_error_handler(function ($errno, $errstr, $errfile, $errline) {
    echo "$errstr on line $errline\n";
});
class Q {
    public $pu = 0;
    function run() {
        for ($i = 0; $i < 2; $i++) {
            $this->pu--;
        }
    }
}
class S extends Q {
    public $pu = 'u';
    function run() {
        parent::run();
    }
}
$objs = [new Q, new S];
for ($k = 0; $k < 4; $k++) {
    foreach ($objs as $o) {
        $o->run();
    }
}
var_dump($objs[0]->pu, $objs[1]->pu);
?>
--EXPECT--
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
Decrement on non-numeric string has no effect and is deprecated on line 9
int(-8)
string(1) "u"
