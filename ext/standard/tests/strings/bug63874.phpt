--TEST--
Bug #63874 (Segfault if php_strip_whitespace has heredoc)
--FILE--
<?php
echo php_strip_whitespace(__FILE__);

return <<<A
a
A;
?>
--EXPECT--
<?php
echo php_strip_whitespace(__FILE__); return <<<A
a
A;
?>
