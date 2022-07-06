--TEST--
Bug #74873 (Minor BC break: PCRE_JIT changes output of preg_match())
--FILE--
<?php
var_dump(preg_match('/\S+/', 'foo bar', $matches, 0, 99999));
?>
--EXPECT--
bool(false)
