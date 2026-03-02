--TEST--
Bug #68996 (Invalid free of CG(interned_empty_string))
--INI--
html_errors=1
--FILE--
<?php
fopen("\xfc\x63", "r");
?>
--EXPECTF--
<br />
<b>Warning</b>:  fopen(�c): Failed to open stream: No such file or directory in <b>%sbug68996.php</b> on line <b>2</b><br />
