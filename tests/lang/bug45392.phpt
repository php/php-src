--TEST--
Bug #45392 (ob_start()/ob_end_clean() and memory_limit)
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}
--FILE--
<?php
echo __LINE__ . "\n";
ini_set('memory_limit', "2M");
ob_start();
$i = 0;
while($i++ < 5000)  {
  echo str_repeat("may not be displayed ", 42);
}
ob_end_clean();
?>
--EXPECTF--
2

Fatal error: Allowed memory size of %d bytes exhausted%s
