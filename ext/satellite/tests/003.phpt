--TEST--
Get a value from an object of type Random
--SKIPIF--
<?php if (!extension_loaded("satellite")) print "skip";
// internet connection needed, try to lookup www.php.net
exec("host www.php.net 2>/dev/null", $output, $rv);
if ($rv) print "skip";
?>
--POST--
--GET--
--FILE--
<?php
//vim: syntax=php
satellite_load_idl("random.idl");
$ior = "IOR:000000000000000f49444c3a52616e646f6d3a312e3000000000000100000000000000500001000000000016706c616e7874792e6473672e63732e7463642e69650006220000002c3a5c706c616e7874792e6473672e63732e7463642e69653a52616e646f6d3a303a3a49523a52616e646f6d00";
$obj = new OrbitObject($ior);
$value = $obj->lrand48();
echo "Random value retrieved";
?>
--EXPECT--
Random value retrieved
