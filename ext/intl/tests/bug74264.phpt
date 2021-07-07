--TEST--
Bug #74264 (grapheme_sttrpos() broken for negative offsets)
--EXTENSIONS--
intl
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
