--TEST--
Bug #80447 (Strange out of memory error when running with JIT)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function createTree($depth) {
    if (!$depth) {
        return [null, null];
    }
    $depth--;

    return [
        createTree($depth),
        createTree($depth)
    ];
}

function checkTree($treeNode) {
    return 1
        + ($treeNode[0][0] === null ? 1 : checkTree($treeNode[0]))
        + ($treeNode[1][0] === null ? 1 : checkTree($treeNode[1]));
}

$tree = createTree(12);
var_dump(checkTree($tree));
?>
--EXPECT--
int(8191)
