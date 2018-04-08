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
Deprecated: Directive 'track_errors' is deprecated in Unknown on line 0

Notice: Undefined variable: cos in %sbug54585.php on line 3
ok
