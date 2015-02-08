--TEST--
Bug #68996 (Invalid free of CG(interned_empty_string))
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") !== "0")
    print "skip Need Zend MM disabled";
?>
--INI--
html_errors=1
--FILE--
<?php
fopen("\xfc\x63", "r");
?>
--EXPECTF--
<br />
<b>Warning</b>:  : failed to open stream: No such file or directory in <b>%sbug68996.php</b> on line <b>%d</b><br />

