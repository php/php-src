--TEST--
Bug #75273 (php_zlib_inflate_filter() may not update bytes_consumed)
--SKIPIF--
<?php
if (!extension_loaded('zlib')) die('skip zlib extension not available');
?>
--FILE--
<?php
function non_repeating_str($len = 8192) {
    $ret = '';
    mt_srand(1);
    $iterations = (int) ($len / 256) + 1;
    for ($i = 0; $i < $iterations; $i++) {
        $haves = array();
        $cnt = 0;
        while ($cnt < 256) {
            $j = mt_rand(0, 255);
            if (!isset($haves[$j])) {
                $haves[$j] = $j;
                $cnt++;
                $ret .= chr($j);
            }
        }
    }
    return substr($ret, 0, $len);
}

$base_len = 32768 - 23 /*overhead*/;

$stream = fopen('php://memory', 'rb+');

for ($i = 1; $i <= 8; $i++) {
    $in_data = non_repeating_str($base_len + $i);

    $deflate_filter = stream_filter_append($stream, 'zlib.deflate',  STREAM_FILTER_WRITE, ['window' => 16 + 15]);
    rewind($stream);
    fwrite($stream, $in_data);
    stream_filter_remove($deflate_filter);

    rewind($stream);
    $out_data = stream_get_contents($stream);
    $out_data_len = strlen($out_data);

    $inflate_filter = stream_filter_prepend($stream, 'zlib.inflate',  STREAM_FILTER_WRITE, ['window' => 16 + 15]);
    rewind($stream);
    $fwrite_len = fwrite($stream, $out_data);
    stream_filter_remove($inflate_filter);

    if ($out_data_len !== $fwrite_len) {
        echo "bug i=$i out_data_len=$out_data_len fwrite_len=$fwrite_len\n";
    }
}

fclose($stream);
?>
===DONE===
--EXPECT--
===DONE===
