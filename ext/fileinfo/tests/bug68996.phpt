--TEST--
Bug #68996 (Invalid free of CG(interned_empty_string))
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--INI--
html_errors=1
--FILE--
<?php
finfo_open(FILEINFO_MIME_TYPE, "\xfc\x63");
?>
--EXPECTF--
<br />
<b>Warning</b>:  finfo_open(%s�c): Failed to open stream: No such file or directory in <b>%sbug68996.php</b> on line <b>%d</b><br />
<br />
<b>Warning</b>:  finfo_open(%s�c): Failed to open stream: No such file or directory in <b>%sbug68996.php</b> on line <b>%d</b><br />
<br />
<b>Warning</b>:  finfo_open(): Failed to load magic database at '%s�c'. in <b>%sbug68996.php</b> on line <b>%d</b><br />
