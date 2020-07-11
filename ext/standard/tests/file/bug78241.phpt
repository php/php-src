--TEST--
Bug #78241 (touch() does not handle dates after 2038 in PHP 64-bit)
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die('skip this test is for Windows platforms only');
if (PHP_INT_SIZE != 8) die('skip this test is for 64bit platforms only');
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$filename = __DIR__ . '/bug78241.txt';
for ($i = 2037; $i <= 2040; $i++) {
  $t = mktime(1, 1 , 1, 1, 1, $i);
  echo 'Date: '.date('D, d M Y H:i:s', $t), PHP_EOL;
  touch($filename, $t);
  clearstatcache(true, $filename);
  $file = filemtime($filename);
  echo 'File: '.date('D, d M Y H:i:s', $file), PHP_EOL, PHP_EOL;
}
?>
--EXPECT--
Date: Thu, 01 Jan 2037 01:01:01
File: Thu, 01 Jan 2037 01:01:01

Date: Fri, 01 Jan 2038 01:01:01
File: Fri, 01 Jan 2038 01:01:01

Date: Sat, 01 Jan 2039 01:01:01
File: Sat, 01 Jan 2039 01:01:01

Date: Sun, 01 Jan 2040 01:01:01
File: Sun, 01 Jan 2040 01:01:01
