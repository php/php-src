--TEST--
gost
--SKIPIF--
<?php extension_loaded('hash') or die('skip'); ?>
--FILE--
<?php
echo hash('gost', ''), "\n";
echo hash('gost', 'The quick brown fox jumps over the lazy dog'), "\n";
echo hash('gost', 'The quick brown fox jumps over the lazy cog'), "\n";
echo hash('gost', str_repeat('a', 31)), "\n";
echo hash('gost', str_repeat('a', 32)), "\n";
echo hash('gost', str_repeat('a', 33)), "\n";
?>
--EXPECT--
ce85b99cc46752fffee35cab9a7b0278abb4c2d2055cff685af4912c49490f8d
77b7fa410c9ac58a25f49bca7d0468c9296529315eaca76bd1a10f376d1f4294
a3ebc4daaab78b0be131dab5737a7f67e602670d543521319150d2e14eeec445
03840d6348763f11e28e7b1ecc4da0cdf7f898fa555b928ef684c6c5b8f46d9f
fd1b746d9397e78edd311baef391450434271e02816caa37680d6d7381c79d4e
715e59cdc8ebde9fdf0fe2a2e811b3bf7f48209a01505e467d2cd2aa2bbb5ecf
