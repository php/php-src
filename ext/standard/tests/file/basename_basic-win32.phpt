--TEST--
basename() basic functionality
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') {
    die('skip Windows only basename tests');
}
?>
--FILE--
<?php

$file_paths = array (
  /* simple paths (forward slashes) */
  "bar",
  "/foo/bar",
  "foo/bar",
  "/bar",

  /* simple paths with trailing slashes (forward slashes) */
  "bar/",
  "/bar/",
  "/foo/bar/",
  "foo/bar/",
  "/bar/",

  /* simple paths (backslashes) */
  "bar",
  "\\foo\\bar",
  "foo\\bar",
  "\\bar",

  /* simple paths with trailing slashes (backslashes) */
  "bar\\",
  "\\bar\\",
  "\\foo\\bar\\",
  "foo\\bar\\",
  "\\bar\\",

  /* paths with numeric strings */
  "10.5\\10.5",
  "10.5/10.5",
  "10.5",
  "105",
  "/10.5",
  "\\10.5",
  "10.5/",
  "10.5\\",
  "10/10.zip",
  "0",
  '0',

  /* path with spaces */
  " ",
  ' ',

  /* empty paths */
  "",
  '',
  NULL,
);

foreach ($file_paths as $file_path) {
    var_dump(basename($file_path));
}

?>
--EXPECT--
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(4) "10.5"
string(4) "10.5"
string(4) "10.5"
string(3) "105"
string(4) "10.5"
string(4) "10.5"
string(4) "10.5"
string(4) "10.5"
string(6) "10.zip"
string(1) "0"
string(1) "0"
string(1) " "
string(1) " "
string(0) ""
string(0) ""
string(0) ""
