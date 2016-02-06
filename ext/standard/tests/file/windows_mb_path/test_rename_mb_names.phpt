--TEST--
Test rename() with a dir for multibyte filenames

--SKIPIF--

<?php

include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";


skip_if_no_required_exts();
skip_if_not_win();


?>

--FILE--

<?php


include dirname(__FILE__) . DIRECTORY_SEPARATOR . "util.inc";



$fw_orig = dirname(__FILE__) . DIRECTORY_SEPARATOR . "data" . DIRECTORY_SEPARATOR . "Ελλάδα.txt";



$fw_copied = dirname(__FILE__) . DIRECTORY_SEPARATOR . "data" . DIRECTORY_SEPARATOR . "Ελλάδα_copy.txt";


$fw_renamed = dirname(__FILE__) . DIRECTORY_SEPARATOR . "data" . DIRECTORY_SEPARATOR . "測試多字節路徑17.txt";



$old_cp = get_active_cp();

set_active_cp(65001);





var_dump(copy($fw_orig, $fw_copied));
var_dump(get_basename_with_cp($fw_copied, get_active_cp(), false));
var_dump(file_exists($fw_copied));

var_dump(rename($fw_copied, $fw_renamed));
var_dump(get_basename_with_cp($fw_renamed, get_active_cp(), false));
var_dump(file_exists($fw_renamed));

var_dump(unlink($fw_renamed));

set_active_cp($old_cp);


?>
===DONE===

--EXPECTF--
Active code page: 65001
bool(true)
string(21) "Ελλάδα_copy.txt"
bool(true)
bool(true)
string(27) "測試多字節路徑17.txt"
bool(true)
bool(true)
Active code page: %d
===DONE===
