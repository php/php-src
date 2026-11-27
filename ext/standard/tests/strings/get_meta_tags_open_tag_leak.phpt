--TEST--
get_meta_tags() does not leak when a tag opens while an attribute value is expected
--FILE--
<?php

$filename = __DIR__."/get_meta_tags_open_tag_leak.html";

file_put_contents($filename, <<<HTML
<meta name="foo" content=<meta>
<meta content="bar" name=<meta>
<meta name="baz" content="qux">
HTML);

var_dump(get_meta_tags($filename));

?>
--CLEAN--
<?php
@unlink(__DIR__."/get_meta_tags_open_tag_leak.html");
?>
--EXPECT--
array(1) {
  ["baz"]=>
  string(3) "qux"
}
