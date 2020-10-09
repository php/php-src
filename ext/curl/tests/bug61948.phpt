--TEST--
Bug #61948 (CURLOPT_COOKIEFILE '' raises open_basedir restriction)
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip curl extension not loaded";
?>
--FILE--
<?php
  $base_dir = __DIR__ . DIRECTORY_SEPARATOR . "bug61948";
  mkdir($base_dir . DIRECTORY_SEPARATOR . "foo", 0755, true);

  ini_set("open_basedir", $base_dir);

  $ch = curl_init();
  var_dump(curl_setopt($ch, CURLOPT_COOKIEFILE, ""));
  var_dump(curl_setopt($ch, CURLOPT_COOKIEFILE, "$base_dir/foo"));
  var_dump(curl_setopt($ch, CURLOPT_COOKIEFILE, "c:/xxx/bar"));
  curl_close($ch);
?>
--CLEAN--
<?php
    $base_dir = __DIR__ . DIRECTORY_SEPARATOR . "bug61948";
    rmdir("$base_dir/foo");
    rmdir($base_dir);
?>
--EXPECTF--
%a
bool(true)

Warning: curl_setopt(): open_basedir restriction in effect. File(c:/xxx/bar) is not within the allowed path(s): (%sbug61948) in %sbug61948.php on line %d
bool(false)
