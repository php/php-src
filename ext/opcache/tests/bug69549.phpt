--TEST--
Bug #69549 (Memory leak with opcache.optimization_level=0xFFFFFFFF)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
$a = array(true);
if($a[0] && false) {
  echo 'test';
}
echo "ok\n";
?>
--EXPECT--
ok
