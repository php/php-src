--TEST--
decode_html() rejects invalid HTML contexts.
--FILE--
<?php
try {
    var_dump(decode_html(NULL, ''));
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
decode_html(): Argument #1 ($context) must be of type HtmlContext, null given
