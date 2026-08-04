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
--EXPECTF--
Fatal error: Uncaught Error: Class "exception" not found in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
