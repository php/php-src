--TEST--
Test mb_convert_encoding() function : Circular references
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_convert_encoding') or die("skip mb_convert_encoding() is not available in this build");
?>
--FILE--
<?php
/*
 * Test basic functionality of mb_convert_encoding()
 */

echo "*** Testing mb_convert_encoding() : Circular references ***\n";

//All strings are the same when displayed in their respective encodings
$sjis_string[] = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
$sjis_string[] = base64_decode('k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==');
$tmp = &$sjis_string;
$sjis_string[] = $tmp;

$jis_string[] = base64_decode('GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==');
$jis_string[] = base64_decode('GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==');
$tmp = &$jis_string;
$jis_string[] = $tmp;

$euc_jp_string[] = base64_decode('xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow==');
$euc_jp_string[] = base64_decode('xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow==');
$tmp = &$euc_jp_string;
$euc_jp_string[] = $tmp;

$utf8_string[] = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');
$utf8_string[] = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');
$tmp = &$utf8_string;
$utf8_string[] = $tmp;


function base64_encode_array($input) {
    foreach ($input as $var) {
        if (is_scalar($var))
            $ret[] = base64_encode($var);
    }
    return $ret;
}

echo "\n-- Convert to JIS --\n";
echo "JIS encoded string in base64:\n";
var_dump(base64_encode_array($jis_string));
echo "Converted Strings:\n";
var_dump(base64_encode_array(mb_convert_encoding($sjis_string, 'JIS', 'SJIS')));
var_dump(base64_encode_array(mb_convert_encoding($euc_jp_string, 'JIS', 'EUC-JP')));
var_dump(base64_encode_array(mb_convert_encoding($utf8_string, 'JIS', 'UTF-8')));

echo "\n-- Convert to EUC-JP --\n";
echo "EUC-JP encoded string in base64:\n";
var_dump(base64_encode_array($euc_jp_string));
echo "Converted Strings:\n";
var_dump(base64_encode_array(mb_convert_encoding($sjis_string, 'EUC-JP', 'SJIS')));
var_dump(base64_encode_array(mb_convert_encoding($jis_string, 'EUC-JP', 'JIS')));
var_dump(base64_encode_array(mb_convert_encoding($utf8_string, 'EUC-JP', 'UTF-8')));

echo "\n-- Convert to SJIS --\n";
echo "SJIS encoded string in base64:\n";
var_dump(base64_encode_array($sjis_string));
echo "Converted Strings:\n";
var_dump(base64_encode_array(mb_convert_encoding($jis_string, 'SJIS', 'JIS')));
var_dump(base64_encode_array(mb_convert_encoding($euc_jp_string, 'SJIS', 'EUC-JP')));
var_dump(base64_encode_array(mb_convert_encoding($utf8_string, 'SJIS', 'UTF-8')));

echo "\n-- Convert to UTF-8 --\n";
echo "UTF-8 encoded string in base64:\n";
var_dump(base64_encode_array($utf8_string));
echo "Converted Strings:\n";
var_dump(base64_encode_array(mb_convert_encoding($sjis_string, 'UTF-8', 'SJIS')));
var_dump(base64_encode_array(mb_convert_encoding($jis_string, 'UTF-8', 'JIS')));
var_dump(base64_encode_array(mb_convert_encoding($euc_jp_string, 'UTF-8', 'EUC-JP')));

echo "Done";
?>
--EXPECTF--
*** Testing mb_convert_encoding() : Circular references ***

-- Convert to JIS --
JIS encoded string in base64:
array(2) {
  [0]=>
  string(68) "GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg=="
  [1]=>
  string(68) "GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg=="
}
Converted Strings:

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(68) "GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg=="
  [1]=>
  string(68) "GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg=="
}

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(68) "GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg=="
  [1]=>
  string(68) "GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg=="
}

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(68) "GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg=="
  [1]=>
  string(68) "GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg=="
}

-- Convert to EUC-JP --
EUC-JP encoded string in base64:
array(2) {
  [0]=>
  string(52) "xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow=="
  [1]=>
  string(52) "xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow=="
}
Converted Strings:

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(52) "xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow=="
  [1]=>
  string(52) "xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow=="
}

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(52) "xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow=="
  [1]=>
  string(52) "xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow=="
}

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(52) "xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow=="
  [1]=>
  string(52) "xvzL3LjspcalraW5pcikx6S5oaMwMTIzNKO1o7ajt6O4o7mhow=="
}

-- Convert to SJIS --
SJIS encoded string in base64:
array(2) {
  [0]=>
  string(52) "k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg=="
  [1]=>
  string(52) "k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg=="
}
Converted Strings:

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(52) "k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg=="
  [1]=>
  string(52) "k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg=="
}

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(52) "k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg=="
  [1]=>
  string(52) "k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg=="
}

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(52) "k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg=="
  [1]=>
  string(52) "k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg=="
}

-- Convert to UTF-8 --
UTF-8 encoded string in base64:
array(2) {
  [0]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
  [1]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
}
Converted Strings:

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
  [1]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
}

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
  [1]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
}

Warning: mb_convert_encoding(): Cannot convert recursively referenced values in %s on line %d
array(2) {
  [0]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
  [1]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
}
Done
