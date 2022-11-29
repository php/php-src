--TEST--
Fix GH-9186 @strict-properties can be bypassed using unserialization
--FILE--
<?php

try {
    unserialize('O:17:"Random\Randomizer":1:{i:0;a:2:{s:3:"foo";N;s:6:"engine";O:32:"Random\Engine\Xoshiro256StarStar":2:{i:0;a:0:{}i:1;a:4:{i:0;s:16:"7520fbc2d6f8de46";i:1;s:16:"84d2d2b9d7ba0a34";i:2;s:16:"d975f36db6490b32";i:3;s:16:"c19991ee16785b94";}}}}');
} catch (Exception $error) {
    echo $error->getMessage() . "\n";
}

?>
--EXPECT--
Invalid serialization data for Random\Randomizer object
