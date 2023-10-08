--TEST--
CLI -a and libedit
--EXTENSIONS--
readline
--SKIPIF--
<?php
include "skipif.inc";
if (readline_info('done') !== NULL) {
    die ("skip need readline support using libedit");
}
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$ini = getenv('TEST_PHP_EXTRA_ARGS');
$descriptorspec = [['pipe', 'r'], STDOUT, STDERR];

$codes = array();

$codes[1] = <<<EOT
echo 'Hello world';
exit
EOT;

$codes[] = <<<EOT
echo 'multine
single
quote';
exit
EOT;

$codes[] = <<<EOT
echo <<<HEREDOC
Here
comes
the
doc
HEREDOC;
EOT;

$codes[] = <<<EOT
if (0) {
    echo "I'm not there";
}
echo "Done";
EOT;

$codes[] = <<<EOT
function a_function_with_some_name() {
    echo "I was called!";
}
a_function_w	);
EOT;

foreach ($codes as $key => $code) {
    echo "\n--------------\nSnippet no. $key:\n--------------\n";
    $proc = proc_open("$php $ini -a", $descriptorspec, $pipes);
    fwrite($pipes[0], $code);
    fclose($pipes[0]);
    proc_close($proc);
}

echo "\nDone\n";
?>
--EXPECT--
--------------
Snippet no. 1:
--------------
Interactive shell

Hello world

--------------
Snippet no. 2:
--------------
Interactive shell

multine
single
quote

--------------
Snippet no. 3:
--------------
Interactive shell

Here
comes
the
doc

--------------
Snippet no. 4:
--------------
Interactive shell

Done

--------------
Snippet no. 5:
--------------
Interactive shell


Parse error: Unmatched ')' in php shell code on line 1

Done
