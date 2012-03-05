--TEST--
bug #61287 - inflate needs the terminating null byte
--SKIPIF--
<?php extension_loaded("zlib") or die("SKIP need zlib");
--FILE--
<?php
$array = array(
    'region_id' => 1,
    'discipline' => 23,
    'degrees' => array(),
    'country_id' => 27
);

$serialized = serialize($array);

$deflated = gzdeflate($serialized, 9);
$inflated = gzinflate($deflated);

echo strlen($inflated),"\n";
?>
Done
--EXPECT--
92
Done
