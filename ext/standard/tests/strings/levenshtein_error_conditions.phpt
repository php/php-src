--TEST--
levenshtein() former error conditions
--FILE--
<?php

// levenshtein no longer has a maximum string length limit.

echo '--- String 1 ---' . \PHP_EOL;
var_dump(levenshtein('AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsu', 'A'));
try {
    var_dump(levenshtein('AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuv', 'A'));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
echo '--- String 2 ---' . \PHP_EOL;
var_dump(levenshtein('A', 'AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsu'));
try {
    var_dump(levenshtein('A', 'AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuv'));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

// TODO ValueError for negative costs?
// var_dump(levenshtein("", "", -1, -1, -1));

?>
--EXPECT--
--- String 1 ---
int(254)
int(255)
--- String 2 ---
int(254)
int(255)
