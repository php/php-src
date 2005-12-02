--TEST--
snefru
--SKIPIF--
<?php extension_loaded('hash') or die('skip'); ?>
--FILE--
<?php
echo hash('snefru', ''), "\n";
echo hash('snefru', 'The quick brown fox jumps over the lazy dog'), "\n";
echo hash('snefru', 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'), "\n";
echo hash('snefru', 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'), "\n";
echo hash('snefru', 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'), "\n";
?>
--EXPECT--
8617f366566a011837f4fb4ba5bedea2b892f3ed8b894023d16ae344b2be5881
674caa75f9d8fd2089856b95e93a4fb42fa6c8702f8980e11d97a142d76cb358
94682bc46e5fbb8417e2f3e10ed360484048d946bb8cbb0ea4cad2700dbeaab0
c54c602ac46383716ee7200a76c9c90a7b435bbe31d13f04e0b00a7ea5c347fa
7a8539c59e192e8d70b1ab82aa86a1b54560d42020bda4e00ddd6d048fe3bcaa
