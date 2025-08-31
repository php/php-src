--TEST--
Bug #78814 (strip_tags allows / in tag name => whitelist bypass)
--FILE--
<?php
echo strip_tags("<s/trong>b</strong>", "<strong>");
?>
--EXPECT--
b</strong>
