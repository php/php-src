--TEST--
GH-23106 (mb_strpos() reads past the end of a haystack ending in a truncated UTF-8 sequence)
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_strpos("AA\xf0\x90", "xyz", 3));
var_dump(mb_strpos("AA\xf0\x90", "x", 3));
var_dump(mb_strrpos("AA\xf0\x90", "A", 3));
var_dump(mb_strrpos("AA\xf0\x90", "A", -1));
try {
    mb_strpos("AA\xf0\x90", "xyz", 4);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
int(1)
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
