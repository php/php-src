--TEST--
Bug #76942 U_ARGUMENT_TYPE_MISMATCH
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
?>
--FILE--
<?php

$locale = 'nl';
$message = '{0,number,#,###.##} MB';
$vars = [
    7.1234
];

$formatter = new MessageFormatter($locale, $message);
if (!$formatter) {
    throw new Exception(intl_get_error_message(), intl_get_error_code());
}

$result = $formatter->format($vars);
if ($result === false) {
    throw new Exception($formatter->getErrorMessage(), $formatter->getErrorCode());
}

var_dump($result);

?>
==DONE==
--EXPECT--
string(7) "7,12 MB"
==DONE==
