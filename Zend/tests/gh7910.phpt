--TEST--
GH-7910 (rename fails on Windows if the target is being executed)
--FILE--
<?php
$content = file_get_contents(__FILE__);
file_put_contents(__DIR__ . "/gh7910.php.bak", $content);
var_dump(rename(__DIR__ . "/gh7910.php.bak", __FILE__));
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh7910.php.bak");
?>
--EXPECT--
bool(true)
