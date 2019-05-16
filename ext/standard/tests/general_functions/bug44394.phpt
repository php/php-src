--TEST--
Bug #44394 (Last two bytes missing from output)
--INI--
session.trans_sid_tags="a=href,area=href,frame=src,form="
url_rewriter.tags="a=href,area=href,frame=src,form="
--FILE--
<?php

$string = "<a href='a?q=1'>asd</a>";

output_add_rewrite_var('a', 'b');

echo $string;

ob_flush();

ob_end_clean();

?>
--EXPECT--
<a href='a?q=1&a=b'>asd</a>
