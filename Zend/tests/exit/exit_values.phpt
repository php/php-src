--TEST--
exit(false);
--FILE--
<?php

function zend_test_var_export($value) {
    if ($value === PHP_INT_MIN) {
        return "PHP_INT_MIN";
    }
    if ($value === PHP_INT_MAX) {
        return "PHP_INT_MAX";
    }
    if (is_array($value)) {
        return "[]";
    }
    if (is_resource($value)) {
        return "STDERR";
    }
    if ($value instanceof stdClass) {
        return "new stdClass()";
    }
    return var_export($value, true);
}

$values = [
    null,
    false,
    true,
    0,
    1,
    20,
    10.0,
    15.5,
    "Hello world",
    [],
    STDERR,
    new stdClass(),
];

const FILE_PATH = __DIR__ . '/exit_values.inc';
const FILE_CONTENT = <<<'TEMPLATE'
<?php
try {
    exit(VALUE);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

TEMPLATE;

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$command = $php . ' ' . escapeshellarg(FILE_PATH);

foreach ([FILE_CONTENT, str_replace('exit', 'die', FILE_CONTENT)] as $code) {
    foreach ($values as $value) {
        echo 'Using ', zend_test_var_export($value), ' as value:', PHP_EOL;
        $output = [];
        $content = str_replace('VALUE', zend_test_var_export($value), $code);
        file_put_contents(FILE_PATH, $content);
        exec($command, $output, $exit_status);
        echo 'Exit status is: ', $exit_status, PHP_EOL,
             'Output is:', PHP_EOL, join($output), PHP_EOL;
    }

    echo 'As a statement:', PHP_EOL;
    $output = [];
    $content = str_replace('(VALUE)', '', $code);
    exec($command, $output, $exit_status);
    echo 'Exit status is: ', $exit_status, PHP_EOL,
         'Output is:', PHP_EOL, join($output), PHP_EOL;
}

?>
--CLEAN--
<?php
const FILE_PATH = __DIR__ . '/exit_values.inc';
@unlink(FILE_PATH);
?>
--EXPECTF--
Using NULL as value:
Exit status is: 0
Output is:

Using false as value:
Exit status is: 0
Output is:

Using true as value:
Exit status is: 0
Output is:
1
Using 0 as value:
Exit status is: 0
Output is:

Using 1 as value:
Exit status is: 1
Output is:

Using 20 as value:
Exit status is: 20
Output is:

Using 10.0 as value:
Exit status is: 0
Output is:
10
Using 15.5 as value:
Exit status is: 0
Output is:
15.5
Using 'Hello world' as value:
Exit status is: 0
Output is:
Hello world
Using [] as value:
Exit status is: 0
Output is:
Warning: Array to string conversion in %s on line 3Array
Using STDERR as value:
Exit status is: 0
Output is:
Resource id #3
Using new stdClass() as value:
Exit status is: 0
Output is:
Error: Object of class stdClass could not be converted to string
As a statement:
Exit status is: 0
Output is:
Error: Object of class stdClass could not be converted to string
Using NULL as value:
Exit status is: 0
Output is:

Using false as value:
Exit status is: 0
Output is:

Using true as value:
Exit status is: 0
Output is:
1
Using 0 as value:
Exit status is: 0
Output is:

Using 1 as value:
Exit status is: 1
Output is:

Using 20 as value:
Exit status is: 20
Output is:

Using 10.0 as value:
Exit status is: 0
Output is:
10
Using 15.5 as value:
Exit status is: 0
Output is:
15.5
Using 'Hello world' as value:
Exit status is: 0
Output is:
Hello world
Using [] as value:
Exit status is: 0
Output is:
Warning: Array to string conversion in %s on line 3Array
Using STDERR as value:
Exit status is: 0
Output is:
Resource id #3
Using new stdClass() as value:
Exit status is: 0
Output is:
Error: Object of class stdClass could not be converted to string
As a statement:
Exit status is: 0
Output is:
Error: Object of class stdClass could not be converted to string
