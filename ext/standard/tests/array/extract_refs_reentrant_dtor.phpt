--TEST--
extract() EXTR_REFS: destructor freeing the target symbol during the ref write (GH-18209 siblings)
--FILE--
<?php
// EXTR_IF_EXISTS path: php_extract_ref_if_exists
class A {
    public function __destruct() {
        unset($GLOBALS['b']);
    }
}
$b = new A();
$r1 = 'r1';
extract(['b' => &$r1], EXTR_REFS | EXTR_IF_EXISTS);
var_dump($b ?? 'b-unset');

// EXTR_PREFIX_ALL path: php_extract_ref_prefix_all
class B {
    public function __destruct() {
        unset($GLOBALS['p_c']);
    }
}
$p_c = new B();
$r2 = 'r2';
extract(['c' => &$r2], EXTR_REFS | EXTR_PREFIX_ALL, 'p');
var_dump($p_c ?? 'p_c-unset');

echo "done\n";
?>
--EXPECT--
string(7) "b-unset"
string(9) "p_c-unset"
done
