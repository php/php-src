--TEST--
Bug #63377 (Segfault on output buffer > 2GB)
--SKIPIF--
<?php
if (PHP_INT_SIZE == 4) {
  die('skip Not for 32-bits OS');
}

$zend_mm_enabled = getenv("USE_ZEND_ALLOC");
if ($zend_mm_enabled === "0") {
    die("skip Zend MM disabled");
}

if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
// check the available memory
if (PHP_OS == 'Linux') {
  $lines = file('/proc/meminfo');
  $infos = array();
  foreach ($lines as $line) {
    $tmp = explode(":", $line);
    $index = strtolower($tmp[0]);
    $value = (int)ltrim($tmp[1], " ")*1024;
    $infos[$index] = $value;
  }
  $freeMemory = $infos['memfree']+$infos['buffers']+$infos['cached'];
  if ($freeMemory < 3072*1024*1024) {
    die('skip Not enough memory.');
  }
}
elseif (PHP_OS == 'FreeBSD') {
  $lines = explode("\n",`sysctl -a`);
  $infos = array();
  foreach ($lines as $line) {
    if(!$line){
      continue;
    }
    $tmp = explode(":", $line);
    $index = strtolower($tmp[0]);
    $value = trim($tmp[1], " ");
    $infos[$index] = $value;
  }
  $freeMemory = ($infos['vm.stats.vm.v_inactive_count']*$infos['hw.pagesize'])
                +($infos['vm.stats.vm.v_cache_count']*$infos['hw.pagesize'])
                +($infos['vm.stats.vm.v_free_count']*$infos['hw.pagesize']);
  if ($freeMemory < 3072*1024*1024) {
    die('skip Not enough memory.');
  }
}
?>
--FILE--
<?php
ini_set('memory_limit', '3072M');

ob_start();
for ($i = 0; $i < 22; $i++)  {
        echo str_repeat('a', 100 * 1024 * 1024);
}
ob_end_clean();
echo "okey";
?>
--EXPECTF--
okey
