--TEST--
Bug #55509 (segfault on x86_64 using more than 2G memory)
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
  if ($freeMemory < 2100*1024*1024) {
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
  if ($freeMemory < 2100*1024*1024) {
    die('skip Not enough memory.');
  }
} elseif (PHP_OS == "WINNT") {
  $s = trim(shell_exec("wmic OS get FreeVirtualMemory /Value 2>nul"));
  $freeMemory = explode('=', $s)[1]*1;

  if ($freeMemory < 2.1*1024*1024) {
    die('skip Not enough memory.');
  }
}
?>
--INI--
memory_limit=2100M
--FILE--
<?php
$a1 = str_repeat("1", 1024 * 1024 * 1024 * 0.5);
echo "1\n";
$a2 = str_repeat("2", 1024 * 1024 * 1024 * 0.5);
echo "2\n";
$a3 = str_repeat("3", 1024 * 1024 * 1024 * 0.5);
echo "3\n";
$a4 = str_repeat("4", 1024 * 1024 * 1024 * 0.5);
echo "4\n";
$a5 = str_repeat("5", 1024 * 1024 * 1024 * 0.5);
echo "5\n";
?>
--EXPECTF--
1
2
3
4

Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %sbug55509.php on line %d
