--TEST--
GH-25700 (destructor triggers the auto-global creation again)
--INI--
auto_globals_jit=1
variables_order=GPCS
--FILE--
<?php
class D {
    public function __destruct() {
        eval('$_ENV;');
    }
}
extract(['_ENV' => ['pad' => str_repeat('A', 64), 'boom' => new D()]]);
eval('$_ENV;');
var_dump($GLOBALS['_ENV']);
?>
--EXPECT--
array(0) {
}
