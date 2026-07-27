--TEST--
GH-21731: Xoshiro256StarStar::__unserialize() must reject the all-zero state
--FILE--
<?php

try {
    var_dump(unserialize('O:32:"Random\Engine\Xoshiro256StarStar":2:{i:0;a:0:{}i:1;a:4:{i:0;s:16:"0000000000000000";i:1;s:16:"0000000000000000";i:2;s:16:"0000000000000000";i:3;s:16:"0000000000000000";}}'));
} catch (\Exception $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Invalid serialization data for Random\Engine\Xoshiro256StarStar object
