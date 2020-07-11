--TEST--
levenshtein() error conditions
--FILE--
<?php

echo '--- String 1 ---' . \PHP_EOL;
var_dump(levenshtein('AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsu', 'A'));
try {
    var_dump(levenshtein('AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuv', 'A'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
echo '--- String 2 ---' . \PHP_EOL;
var_dump(levenshtein('A', 'AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsu'));
try {
    var_dump(levenshtein('A', 'AbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrstuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuvwxyzAbcdefghijklmnopqrtsuv'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// TODO ValueError for negative costs?
// var_dump(levenshtein("", "", -1, -1, -1));

?>
--EXPECT--
--- String 1 ---
int(254)
levenshtein(): Argument #1 ($str1) must be less than 256 characters
--- String 2 ---
int(254)
levenshtein(): Argument #2 ($str2) must be less than 256 characters
