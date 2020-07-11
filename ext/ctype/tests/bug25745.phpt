--TEST--
Bug #25745 (ctype functions fail with non-ascii characters)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$funcs = array(
    "ctype_alnum", "ctype_alpha", "ctype_cntrl", "ctype_digit",
    "ctype_graph", "ctype_lower", "ctype_print", "ctype_punct",
    "ctype_space", "ctype_upper", "ctype_xdigit"
);

foreach ($funcs as $ctype_func) {
    for ($i = 0; $i < 256; $i++) {
        $a = $ctype_func($i);
        $b = $ctype_func(chr($i));
        if ($a != $b) {
            echo "broken... $ctype_func($i) = $a, $ctype_func(chr($i)) = $b\n";
            exit;
        }
    }
}
echo "ok\n";
?>
--EXPECT--
ok
