--TEST--
Random: Engine: Xoshiro256StarStar: The serialization output must be stable
--FILE--
<?php

use Random\Engine\Xoshiro256StarStar;

echo serialize(new Xoshiro256StarStar(1234));

?>
--EXPECT--
O:32:"Random\Engine\Xoshiro256StarStar":2:{i:0;a:0:{}i:1;a:4:{i:0;s:16:"db1c182f1bf60cbb";i:1;s:16:"2465f04d36a1c797";i:2;s:16:"da25c09be4fabe33";i:3;s:16:"33a0d052f241624e";}}
