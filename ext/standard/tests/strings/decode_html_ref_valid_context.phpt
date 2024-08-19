--TEST--
decode_html_ref() rejects invalid HTML contexts.
--FILE--
<?php
try {
    $matched_bytes = null;
    var_dump(decode_html_ref(NULL, '', 0, $matched_bytes));
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
decode_html_ref(): Argument #1 ($context) must be of type HtmlContext, null given
