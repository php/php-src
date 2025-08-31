--TEST--
GdImage instances must not be serialized
--EXTENSIONS--
gd
--FILE--
<?php

    $img_src = imagecreatetruecolor(32, 32);
    var_dump(serialize($img_src));

?>
--EXPECTF--
Fatal error: Uncaught Exception: Serialization of 'GdImage' is not allowed in %s:%d
Stack trace:
#0 %s(%d): serialize(Object(GdImage))
#1 {main}
  thrown in %s on line %d
