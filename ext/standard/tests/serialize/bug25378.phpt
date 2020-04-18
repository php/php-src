--TEST--
Bug #25378 (unserialize() crashes with invalid data)
--FILE--
<?php
var_dump(unserialize("s:-1:\"\";"));
var_dump(unserialize("i:823"));
var_dump(unserialize("O:8:\"stdClass :0:{}"));
var_dump(unserialize("O:8:\"stdClass\"+0:{}"));
var_dump(unserialize("O:1000:\"stdClass\":0:{}"));
var_dump(unserialize("a:2:{i:0;s:2:\"12\":"));
var_dump(unserialize("a:2:{i:0;s:2:\"12\";i:1;s:3000:\"123"));
var_dump(unserialize("a:2:{i:0;s:2:\"12\"+i:1;s:3:\"123\";}"));
var_dump(unserialize("a:2:{i:0;s:2:\"12\";i:1;s:3:\"123\";"));
var_dump(unserialize("s:3000:\"123\";"));
var_dump(unserialize("s:3000:\"123"));
var_dump(unserialize("s:3:\"123;"));
var_dump(unserialize("s:0:\"123\";"));
?>
--EXPECTF--
Notice: unserialize(): Error at offset 0 of 8 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 0 of 5 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 13 of 19 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 14 of 19 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 2 of 22 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 17 of 18 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 24 of 33 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 17 of 33 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 32 of 32 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 2 of 13 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 2 of 11 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 8 of 9 bytes in %sbug25378.php on line %d
bool(false)

Notice: unserialize(): Error at offset 5 of 10 bytes in %sbug25378.php on line %d
bool(false)
