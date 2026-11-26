--TEST--
GH-25700 (destructor overwrites the lazily created auto-global)
--INI--
auto_globals_jit=1
variables_order=GPCS
--FILE--
<?php
class D {
    public function __destruct() {
        $GLOBALS['_ENV'] = 1;
    }
}
extract(['_ENV' => ['pad' => str_repeat('A', 64), 'boom' => new D()]]);
eval('$_ENV;');
var_dump($GLOBALS['_ENV']);
?>
--EXPECT--
int(1)
