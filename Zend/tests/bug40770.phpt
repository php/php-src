--TEST--
Bug #40770 Apache child exits when PHP memory limit reached 
--INI--
memory_limit=64M
--FILE--
<?php
ini_set('display_errors',true);
$mb=148;
$var = '';
for ($i=0; $i<=$mb; $i++) {
        $var.= str_repeat('a',1*1024*1024);
}
?>
--EXPECTF--
Fatal error: Allowed memory size of 67108864 bytes exhausted%s(tried to allocate %d bytes) in %sbug40770.php on line 6
