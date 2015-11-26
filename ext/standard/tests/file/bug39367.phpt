--TEST--
Bug #39367 (clearstatcache() doesn't clear realpath cache)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip no symlinks on Windows');
}
?>
--FILE--
<?php
function test() {
  unlink('/tmp/1link');
  unlink('/tmp/1tmp');
  unlink('/tmp/testfile1');

  file_put_contents('/tmp/testfile1', 'ok');
  symlink('/tmp/testfile1', '/tmp/1tmp');
  rename('/tmp/1tmp', '/tmp/1link');
  echo file_get_contents('/tmp/1link')."\n";

  unlink('/tmp/1link');
  clearstatcache(true);

  echo file_get_contents('/tmp/1link')."\n";

  unlink('/tmp/1link');
  unlink('/tmp/1tmp');
  unlink('/tmp/testfile1');
}
@test();
?>
--EXPECT--
ok
