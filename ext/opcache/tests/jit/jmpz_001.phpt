--TEST--
JIT JMPZ: JMPZ may require code for "smart branch" and at the same time be a target of another JMP.
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
namespace A;

function test() {

    $modelData = array();
    $ret = false ||
        ((is_array($modelData) || $modelData instanceof \Countable) && true) || false;
    return $ret;
}

var_dump(test());
?>
--EXPECT--
bool(true)
