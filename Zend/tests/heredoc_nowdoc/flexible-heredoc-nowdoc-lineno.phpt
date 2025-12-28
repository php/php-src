--TEST--
Flexible heredoc lineno: ensure the compiler globals line number is correct
--FILE--
<?php

$heredoc = <<<EOT
hello world
EOT;

$heredoc = <<<'EOT'
hello world
EOT;

$heredoc = <<<EOT
 hello world
 EOT;

$heredoc = <<<'EOT'
 hello world
 EOT;

try {
	throw new exception();
} catch (Exception $e) {
	var_dump($e->getLine());
}

?>
--EXPECT--
int(20)
