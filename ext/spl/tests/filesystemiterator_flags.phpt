--TEST--
SPL: FilesystemIterator::getFlags() basic tests
--CREDITS--
Joshua Thijssen <jthijssen@noxlogic.nl>
--FILE--
<?php

$it = new FileSystemIterator(".");
printflags($it);

$it->setFlags(FileSystemIterator::CURRENT_AS_SELF |
		FileSystemIterator::KEY_AS_FILENAME |
		FileSystemIterator::SKIP_DOTS | 
		FileSystemIterator::UNIX_PATHS);
printflags($it);

$it->setFlags(-1);
printflags($it);

function printflags($it) {
	printf("%08X\n", $it->getFlags());
	printf("%08X\n", ($it->getFlags() & FileSystemIterator::CURRENT_MODE_MASK));
	printf("%08X\n", ($it->getFlags() & FileSystemIterator::KEY_MODE_MASK));
	printf("%08X\n", ($it->getFlags() & FileSystemIterator::OTHER_MODE_MASK));
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
00003FF0
000000F0
00000F00
00003000
