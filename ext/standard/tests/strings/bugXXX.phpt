--TEST--
Bug #XXXXX: strip_tags strip >< in attributes with allow tag
--FILE--
<?php

echo strip_tags('<ta alt="abc< " >', '<ta>').\PHP_EOL;
echo strip_tags('<ta alt="abc> " >', '<ta>').\PHP_EOL;

echo strip_tags('<ta alt="abc< " ><ta2>inside ta2</ta2>', '<ta><ta2>').\PHP_EOL;

$lts = str_repeat('<',40);
echo strip_tags('<t y="'.$lts.'" /><pippo>xxx</pippo><pippo></pippo><k>hello</k>', '<t><pippo>').\PHP_EOL;
echo strip_tags('<t y="1< 2< 3< 4< 5< 6< 7< 8< 9< 10<" />', '<t>').\PHP_EOL;
echo strip_tags('<t y="<" />', '<t>').\PHP_EOL;
echo strip_tags('<ta alt="abc< " ><ta2>inside ta2</ta2>', '<ta><ta2>').\PHP_EOL;
echo strip_tags('<ta alt="abc<"><ta2>inside ta2</ta2>', '<ta><ta2>').\PHP_EOL;
echo strip_tags('<ta alt="abcdefghilmn"><ta2>inside ta2</ta2>', '<ta2>').\PHP_EOL;

?>
--EXPECT--
<ta alt="abc&lt; " >
<ta alt="abc&gt; " >
<ta alt="abc&lt; " ><ta2>inside ta2</ta2>
<t y="&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;" /><pippo>xxx</pippo><pippo></pippo>hello
<t y="1&lt; 2&lt; 3&lt; 4&lt; 5&lt; 6&lt; 7&lt; 8&lt; 9&lt; 10&lt;" />
<t y="&lt;" />
<ta alt="abc&lt; " ><ta2>inside ta2</ta2>
<ta alt="abc&lt;"><ta2>inside ta2</ta2>
<ta2>inside ta2</ta2>
