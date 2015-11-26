--TEST--
Bug #54585 (track_errors causes segfault)
--INI--
track_errors=On
--FILE--
<?php
function testing($source) {
                unset($source[$cos]);
}
testing($_GET);
echo "ok\n";
?>
--EXPECTF--
Notice: Undefined variable: cos in %sbug54585.php on line 3
ok
