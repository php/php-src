--TEST--
JIT COALESCE: guard in both recorded directions and CV reference
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=tracing
opcache.jit_hot_loop=1
opcache.jit_hot_side_exit=1
--EXTENSIONS--
opcache
--FILE--
<?php
/* Trace recorded with a non-null operand (jump taken), then null hits the guard. */
function taken(array $values) {
    $out = [];
    foreach ($values as $v) {
        $out[] = $v ?? 'null';
    }
    return implode(',', $out);
}

/* Trace recorded with a null operand (fallthrough), then non-null hits the guard. */
function not_taken(array $values) {
    $out = [];
    foreach ($values as $v) {
        $out[] = $v ?? 'null';
    }
    return implode(',', $out);
}

/* The CV is a reference, so the operand must be dereferenced. */
function cv_ref(array $values) {
    $out = [];
    $src = null;
    $r = &$src;
    foreach ($values as $v) {
        $src = $v;
        $out[] = $r ?? 'null';
    }
    return implode(',', $out);
}

echo taken(['a', 'b', 'c', 'd', null, null, 'e', null]), "\n";
echo not_taken([null, null, null, null, 'a', 'b', null, 'c']), "\n";
echo cv_ref(['a', 'b', 'c', 'd', null, null, 'e', null]), "\n";
echo cv_ref([null, null, null, null, 'a', 'b', null, 'c']), "\n";
?>
--EXPECT--
a,b,c,d,null,null,e,null
null,null,null,null,a,b,null,c
a,b,c,d,null,null,e,null
null,null,null,null,a,b,null,c
