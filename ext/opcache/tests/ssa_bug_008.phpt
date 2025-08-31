--TEST--
Incorrect CFG/SSA reconstruction
--EXTENSIONS--
opcache
--FILE--
<?php
if (!is_int($info = gc_collect_cycles()) || ($info < 100)) {
        echo gettype($info)."\n";
}
?>
--EXPECT--
integer
