--TEST--
GH-25700 (Use-after-free when a lazily created auto-global replaces an existing global variable)
--INI--
auto_globals_jit=1
variables_order=GPCS
--FILE--
<?php
class D {
    public function __destruct() {
        $GLOBALS['keep'] = $GLOBALS['_ENV'];
    }
}
extract(['_ENV' => ['pad' => str_repeat('A', 64), 'boom' => new D()]]);
eval('$_ENV;');
var_dump($GLOBALS['keep']);
?>
--EXPECT--
array(0) {
}
