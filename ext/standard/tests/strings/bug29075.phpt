--TEST--
Bug #29075 (strnatcmp() incorrectly handles whitespace)
--FILE--
<?php
    var_dump(
        strnatcmp('foo ', 'foo '),
        strnatcmp('foo', 'foo'),
        strnatcmp(' foo', ' foo')
    );
?>
--EXPECT--
int(0)
int(0)
int(0)
