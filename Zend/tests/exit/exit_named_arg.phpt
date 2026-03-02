--TEST--
Using exit()/die() as function call with a named argument
--FILE--
<?php

$values = [
    12,
    "Goodbye!",
];

const FILE_PATH = __DIR__ . '/exit_named_arg_test.php';
const FILE_CONTENT = <<<'TEMPLATE'
<?php
try {
    exit(status: VALUE);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

TEMPLATE;

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$command = $php . ' --no-php-ini ' . escapeshellarg(FILE_PATH);

foreach ([FILE_CONTENT, str_replace('exit', 'die', FILE_CONTENT)] as $code) {
    foreach ($values as $value) {
        echo 'Using ', var_export($value, true), ' as value:', PHP_EOL;
        $output = [];
        $content = str_replace('VALUE', var_export($value, true), $code);
        file_put_contents(FILE_PATH, $content);
        exec($command, $output, $exit_status);
        echo 'Exit status is: ', $exit_status, PHP_EOL,
             'Output is:', PHP_EOL, join($output), PHP_EOL;
    }
}

?>
--CLEAN--
<?php
const FILE_PATH = __DIR__ . '/exit_named_arg_test.php';
@unlink(FILE_PATH);
?>
--EXPECT--
Using 12 as value:
Exit status is: 12
Output is:

Using 'Goodbye!' as value:
Exit status is: 0
Output is:
Goodbye!
Using 12 as value:
Exit status is: 12
Output is:

Using 'Goodbye!' as value:
Exit status is: 0
Output is:
Goodbye!
