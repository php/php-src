--TEST--
Bug #78441 (Parse error due to heredoc identifier followed by digit)
--FILE--
<?php
echo <<<FOO
FOO4
FOO, PHP_EOL;

echo <<<FOO
bar
FOO4
FOO, PHP_EOL;

echo <<<'FOO'
bar
FOO4
FOO, PHP_EOL;
?>
--EXPECT--
FOO4
bar
FOO4
bar
FOO4
