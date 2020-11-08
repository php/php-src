--TEST--
Bug #70239 Creating a huge array doesn't result in exhausted, but segfault, var 2
--FILE--
<?php
try {
    range(pow(2.0, 100000000), pow(2.0, 100000000) + 1);
} catch (\ValueError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Invalid range supplied: start=inf end=inf
