--TEST--
Bug #54322: Null pointer deref in get_html_translation_table due to information loss in long-to-int conversion
--FILE--
<?php
try {
    var_dump(get_html_translation_table(NAN, 0, "UTF-8") > 0);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
get_html_translation_table(): Argument #1 ($table) must be of type int, float given
