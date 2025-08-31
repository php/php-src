--TEST--
Phar: copy-on-write test 12 [cache_list]
--INI--
default_charset=UTF-8
phar.cache_list={PWD}/copyonwrite12.phar.php
phar.readonly=0
--EXTENSIONS--
phar
--FILE_EXTERNAL--
files/write12.phar
--EXPECTF--
array(2) {
  ["hash"]=>
  string(40) "0163F471460EA74F7636268D28289BF5A1E8BD72"
  ["hash_type"]=>
  string(5) "SHA-1"
}
array(2) {
  ["hash"]=>
  string(32) "%s"
  ["hash_type"]=>
  string(3) "MD5"
}
ok
