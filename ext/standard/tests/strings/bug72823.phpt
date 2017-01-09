--TEST--
Bug #72823 (strtr out-of-bound access)
--FILE--
<?php
var_dump(
    strtr(11, array('aaa' => 'bbb'))
);
?>
===DONE===
--EXPECT--
string(2) "11"
===DONE===
