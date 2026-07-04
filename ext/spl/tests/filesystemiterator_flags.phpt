--TEST--
SPL: FilesystemIterator::getFlags() basic tests
--CREDITS--
Joshua Thijssen <jthijssen@noxlogic.nl>
--FILE--
<?php

$it = new FilesystemIterator(".");
printflags($it);

$it->setFlags(FilesystemIterator::CURRENT_AS_SELF |
        FilesystemIterator::KEY_AS_FILENAME |
        FilesystemIterator::SKIP_DOTS |
        FilesystemIterator::UNIX_PATHS);
printflags($it);

$it->setFlags(-1);
printflags($it);

function printflags($it) {
    printf("%08X\n", $it->getFlags());
    printf("%08X\n", ($it->getFlags() & FilesystemIterator::CURRENT_MODE_MASK));
    printf("%08X\n", ($it->getFlags() & FilesystemIterator::KEY_MODE_MASK));
    printf("%08X\n", ($it->getFlags() & FilesystemIterator::OTHER_MODE_MASK));
}

?>
--EXPECT--
00001000
00000000
00000000
00001000
00003110
00000010
00000100
00003000
00007FF0
000000F0
00000F00
00007000
