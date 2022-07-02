--TEST--
Bug #70239 Creating a huge array doesn't result in exhausted, but segfault, var 1
--FILE--
<?php
try {
    range(0, pow(2.0, 100000000));
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Invalid range supplied: start=0 end=inf
