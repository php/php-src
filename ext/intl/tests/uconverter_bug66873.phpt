--TEST--
Bug #66873 - crash in UConverter with invalid encoding
--EXTENSIONS--
intl
--FILE--
<?php
    $o = new UConverter(1, 1);
    $o->toUCallback(1, 1, 1, $b);
    var_dump($o->getErrorCode());
?>
--EXPECT--
int(27)
