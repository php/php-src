--TEST--
GH-25700 (destructor unsets the lazily created auto-global)
--INI--
auto_globals_jit=1
variables_order=GPCS
--FILE--
<?php
class D {
    public function __destruct() {
        unset($GLOBALS['_ENV']);
    }
}
extract(['_ENV' => ['pad' => str_repeat('A', 64), 'boom' => new D()]]);
eval('$_ENV;');
var_dump(isset($GLOBALS['_ENV']));
?>
--EXPECTF--
Warning: Undefined global variable $_ENV in %s : eval()'d code on line 1
bool(false)
