--TEST--
FR #51368 (php_filter_float does not allow custom thousand separators)
--EXTENSIONS--
filter
--FILE--
<?php
$options = ['flags' => FILTER_FLAG_ALLOW_THOUSAND, 'options' => ['thousand' => ' ']];
var_dump(
    filter_var('1 000', FILTER_VALIDATE_FLOAT, $options),
    filter_var('1 234.567', FILTER_VALIDATE_FLOAT, $options)
);
$options = ['flags' => FILTER_FLAG_ALLOW_THOUSAND, 'options' => ['thousand' => '']];

try {
    filter_var('12345', FILTER_VALIDATE_FLOAT, $options);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
float(1000)
float(1234.567)
filter_var(): "thousand" option cannot be empty
