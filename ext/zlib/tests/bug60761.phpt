--TEST--
checks zlib compression output size is always the same
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--CGI--
1
--FILE--
<?php

// the INI directives from bug #60761 report
ini_set('zlib.output_compression', '4096');
ini_set('zlib.output_compression_level', '9');

// try to duplicate the original bug by running this as a CGI
// test using ob_start and zlib.output_compression(or ob_gzhandler)
// so it follows more of the original code-path than just calling
// gzcompress on CLI or CGI

$lens = array();

for ( $i=0 ; $i < 100 ; $i++ ) {

	// can't use ob_gzhandler with zlib.output_compression
	ob_start();//"ob_gzhandler");
	phpinfo();
	$html = ob_get_clean();

	$len = strlen($html);

	$lens[$len] = $len;
} 
 
$lens = array_values($lens);

echo "Compressed Lengths\n";

// pass == only ONE length for all iterations
//         (length didn't change during run)
// 
// hard to anticipate what 'correct' length should be since
// return value of phpinfo() will vary between installations...
// just check that there is only one length
// 
var_dump($lens); // show lengths to help triage in case of failure
 
// expected headers since its CGI

?>
--EXPECTF--
%s
array(1) {
  [0]=>
  int(%d)
}
