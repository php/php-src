--TEST--
Bug GH-7809 (Cloning a faked SplFileInfo object may segfault)
--FILE--
<?php
class MySplFileInfo extends SplFileInfo {
    public function __construct(string $filename) {}
}

$sfi = new MySplFileInfo("foo");
clone $sfi;
?>
--EXPECT--
