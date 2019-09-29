--TEST--
Bug #71417: fread() does not detect decoding errors from filter zlib.inflate
--SKIPIF--
<?php if (!extension_loaded('zlib')) die ('skip zlib extension not available in this build'); ?>
--FILE--
<?php

function test($case) {
	$plain = "The quick brown fox jumps over the lazy dog.";
	$fn = "bug71417.gz";
	$compressed = (string) gzencode($plain);
	
    if ($case == 1) {
        // 1. Set a random byte in the middle of the compressed data.
        // $ php test-zlib-inflate.php
        // --> read: string(0) ""
        // --> read: string(44) "The quick brown fox jumps over the lazx8dog."
        // $ gzip test-zlib-inflate.gz
        // gzip: test-zlib-inflate.gz: invalid compressed data--crc error
        $compressed[strlen($compressed) - 15] = 'X';
    } else if ($case == 2) {
        // 2. Truncate the compressed data.
        // $ php test-zlib-inflate.php
        // --> read: string(32) "The quick brown fox jumps over t"
        // $ gzip test-zlib-inflate.gz
        // gzip: test-zlib-inflate.gz: unexpected end of file
        $compressed = substr($compressed, 0, strlen($compressed) - 20);
    } else if ($case == 3) {
        // 3. Corrupted final CRC.
        // $ php test-zlib-inflate.php
        // --> read: string(0) ""
        // --> read: string(44) "The quick brown fox jumps over the lazy dog."
        // $ gzip test-zlib-inflate.gz
        // gzip: test-zlib-inflate.gz: invalid compressed data--crc error
        $compressed[strlen($compressed)-5] = 'X';
    } else if ($case == 4) {
        // 4. Corrupted final length.
        // $ php test-zlib-inflate.phpread: string(0) ""
        // read: string(44) "The quick brown fox jumps over the lazy dog."
        // $ gunzip test-zlib-inflate.gz 
        // gzip: test-zlib-inflate.gz: invalid compressed data--length error
        $compressed[strlen($compressed)-2] = 'X';
    }
	
	// The gzdecode() function applied to the corrupted compressed data always
	// detects the error:
	// --> gzdecode(): PHP Fatal error:  Uncaught ErrorException: gzdecode(): data error in ...
	echo "gzdecode(): ", rawurldecode(gzdecode($compressed)), "\n";

	file_put_contents($fn, $compressed);
	
	$r = fopen($fn, "r");
	stream_filter_append($r, 'zlib.inflate', STREAM_FILTER_READ, array('window' => 15+16));
	while (!feof($r)) {
		$s = fread($r, 100);
		echo "read: "; var_dump($s);
	}
	fclose($r);
	unlink($fn);
}

test(1);
test(2);
test(3);
test(4);

?>
--EXPECTF--
gzdecode(): 
Warning: gzdecode(): data error in %s on line %d

read: bool(false)
gzdecode(): 
Warning: gzdecode(): data error in %s on line %d

read: string(32) "The quick brown fox jumps over t"
gzdecode(): 
Warning: gzdecode(): data error in %s on line %d

read: bool(false)
gzdecode(): 
Warning: gzdecode(): data error in %s on line %d

read: bool(false)
