--TEST--
GH-23958: Truncated encoding names must not be accepted
--EXTENSIONS--
mbstring
--FILE--
<?php
foreach (['pa', 'UCS-'] as $name) {
    try {
        mb_http_output($name);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
mb_http_output(): Argument #1 ($encoding) must be a valid encoding, "pa" given
mb_http_output(): Argument #1 ($encoding) must be a valid encoding, "UCS-" given
