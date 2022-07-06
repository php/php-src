--TEST--
Bug #74264 (grapheme_sttrpos() broken for negative offsets)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die("skip intl extension not available");
?>
--FILE--
<?php
foreach (range(-5, -1) as $offset) {
    var_dump(
        grapheme_strrpos('déjàààà', 'à', $offset),
        grapheme_strripos('DÉJÀÀÀÀ', 'à', $offset)
    );        
}
?>
--EXPECT--
bool(false)
bool(false)
int(3)
int(3)
int(4)
int(4)
int(5)
int(5)
int(6)
int(6)
