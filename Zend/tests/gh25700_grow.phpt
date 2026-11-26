--TEST--
GH-25700 (destructor grows the symbol table while the auto-global is replaced)
--INI--
auto_globals_jit=1
variables_order=GPCS
--FILE--
<?php
class D {
    public function __destruct() {
        for ($i = 0; $i < 200; $i++) {
            $GLOBALS["g$i"] = $i;
        }
    }
}
extract(['_ENV' => ['pad' => str_repeat('A', 64), 'boom' => new D()]]);
eval('$_ENV;');
var_dump($GLOBALS['_ENV'], $g199);
?>
--EXPECT--
array(0) {
}
int(199)
