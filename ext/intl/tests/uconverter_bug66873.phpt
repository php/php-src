--TEST--
Bug #66873 - crash in UConverter with invalid encoding
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
    $o = new UConverter(1, 1);
    $o->toUCallback(1, 1, 1, $b);
    var_dump($o->getErrorCode());
--EXPECT--
int(27)
