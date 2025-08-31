--TEST--
Bug #44849 (imagecolorclosesthwb is not available on Windows)
--EXTENSIONS--
gd
--FILE--
<?php
    var_dump(function_exists('imagecolorclosesthwb'));
?>
--EXPECT--
bool(true)
