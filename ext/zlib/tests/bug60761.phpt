--TEST--
checks zlib compression size is always the same no matter how many times its run
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php

ob_start();
phpinfo();
$html = ob_get_clean();

$lens = array();

for ( $i=0 ; $i < 200 ; $i++ ) {
	//zlib.output_compression = On
	//zlib.output_compression_level = 9
	$compressed = gzcompress($html, 9);
	
	$len = strlen($compressed);

	$lens[$len] = $len;
} 
 
$lens = array_values($lens);

echo "Compressed lengths\n";
var_dump($lens);

?>
--EXPECTF--
Compressed lengths
array(1) {
  [0]=>
  int(%d)
}
