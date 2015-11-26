--TEST--
Bug #60350 No string escape code for ESC (ascii 27), normally \e
--FILE--
<?php
$str = "\e";
if (ord($str) == 27) {
    echo "Works";
}
?>
--EXPECT--
Works
