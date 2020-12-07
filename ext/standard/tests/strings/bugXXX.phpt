--TEST--
Bug #XXXXX: strip_tags strip >< in attributes with allow tag
--FILE--
<?php

echo strip_tags('<ta alt="abc< " >', '<ta>').\PHP_EOL;
echo strip_tags('<ta alt="abc> " >', '<ta>').\PHP_EOL;

?>
--EXPECT--
<ta alt="abc&lt; " >
<ta alt="abc&gt; " >
