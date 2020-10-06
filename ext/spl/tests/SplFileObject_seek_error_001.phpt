--TEST--
SplFileObject::seek function - test parameters
--FILE--
<?php
$obj = new SplFileObject(__FILE__);
try {
    $obj->seek(-1);
} catch (\ValueError $e) {
    echo($e->getMessage());
}
?>
--EXPECT--
SplFileObject::seek(): Argument #1 ($line) must be greater than or equal to 0
