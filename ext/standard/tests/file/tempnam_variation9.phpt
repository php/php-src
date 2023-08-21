--TEST--
Test tempnam() function: usage variations - test prefix maximum size
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') {
    die("skip Do not run on Windows");
}
?>
--FILE--
<?php
/* Testing the maximum prefix size */

echo "*** Testing tempnam() maximum prefix size ***\n";
$file_path = __DIR__."/tempnamVar9";
mkdir($file_path);

$pre_prefix = "begin_";
$post_prefix = "_end";
$fixed_length = strlen($pre_prefix) + strlen($post_prefix);
/* An array of prefixes */
$names_arr = [
  $pre_prefix . str_repeat("x", 7) . $post_prefix,
  $pre_prefix . str_repeat("x", 63 - $fixed_length) . $post_prefix,
  $pre_prefix . str_repeat("x", 64 - $fixed_length) . $post_prefix,
  $pre_prefix . str_repeat("x", 65 - $fixed_length) . $post_prefix,
  $pre_prefix . str_repeat("x", 300) . $post_prefix,
];

foreach($names_arr as $i=>$prefix) {
    echo "-- Iteration $i --\n";
    try {
        $file_name = tempnam("$file_path", $prefix);
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
        continue;
    }

    $base_name = basename($file_name);
    echo "File name is => ", $base_name, "\n";
    echo "File name length is => ", strlen($base_name), "\n";

    if (file_exists($file_name)) {
        unlink($file_name);
    }
}
rmdir($file_path);

?>
--CLEAN--
<?php
$file_path = __DIR__."/tempnamVar9";
if (file_exists($file_path)) {
    array_map('unlink', glob($file_path . "/*"));
    rmdir($file_path);
}
?>
--EXPECTF--
*** Testing tempnam() maximum prefix size ***
-- Iteration 0 --
File name is => begin_%rx{7}%r_end%r.{6}%r
File name length is => 23
-- Iteration 1 --
File name is => begin_%rx{53}%r_end%r.{6}%r
File name length is => 69
-- Iteration 2 --
File name is => begin_%rx{54}%r_en%r.{6}%r
File name length is => 69
-- Iteration 3 --
File name is => begin_%rx{55}%r_e%r.{6}%r
File name length is => 69
-- Iteration 4 --
File name is => begin_%rx{57}%r%r.{6}%r
File name length is => 69
