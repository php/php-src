--TEST--
GH-23991 (JIT generates invalid IR for multiple recursive calls converted into loop)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=64M
opcache.jit=1235
opcache.jit_hot_func=1
--EXTENSIONS--
opcache
--FILE--
<?php
function f($value, $recurse, $param = '') {
    if (is_array($value)) {
        if ($recurse) {
            foreach ($value as $k => $v) {
                $value[$k] = f($v, $recurse, $param);
            }
        }
        foreach ($value as $k => $v) {
            if ($undef) {
                $value[$k] = f($v, $recurse, $param);
            }
        }
    }
    return $value;
}
var_dump(f([[]], true));
?>
--EXPECTF--
Warning: Undefined variable $undef in %s on line %d
array(1) {
  [0]=>
  array(0) {
  }
}
