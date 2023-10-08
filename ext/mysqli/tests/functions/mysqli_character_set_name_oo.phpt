--TEST--
function test: mysqli_character_set_name
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";
mysqli_check_skip_test();
?>
--FILE--
<?php
require_once dirname(__DIR__) . "/test_setup/test_helpers.inc";

$link = default_mysqli_connect();

$result = $link->query('SELECT @@character_set_connection AS charset, @@collation_connection AS collation');
$tmp = $result->fetch_assoc();
$result->free_result();

if (!$tmp['charset']) {
    throw new Exception("Cannot determine current character set and collation");
}

$charset = $link->character_set_name();
if ($tmp['charset'] !== $charset) {
    printf("[001] Expecting character set %s/%s, got %s/%s\n", get_debug_type($tmp['charset']), $tmp['charset'], get_debug_type($charset), $charset);
}

$link->close();

try {
    $link->character_set_name();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

print "done!";
?>
--EXPECT--
mysqli object is already closed
done!
