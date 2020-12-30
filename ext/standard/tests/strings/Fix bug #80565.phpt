--TEST--
Bug #XXXXX: strip_tags strip >< in attributes with allow tag
--FILE--
<?php
echo strip_tags('<ta alt="abc< ">', '<ta>').\PHP_EOL;
echo strip_tags('<ta alt="abc> ">', '<ta>').\PHP_EOL;

$lts = '';
$gts = '';
for ($i=0;$i<10;$i++)
{
    $lts.=" $i(<) ";
    $gts.=" $i(>) ";
}
echo strip_tags('<t y="'.$lts.'" /><pippo> xxx </pippo><pippo> yyy </pippo><k> hello!</k>', '<t><pippo>').\PHP_EOL;
echo strip_tags('<t y="'.$gts.'" /><pippo> xxx </pippo><pippo> yyy </pippo><k> hello!</k>', '<t><pippo>').\PHP_EOL;
?>
--EXPECT--
<ta alt="abc&lt; ">
<ta alt="abc&gt; ">
<t y=" 0(&lt;)  1(&lt;)  2(&lt;)  3(&lt;)  4(&lt;)  5(&lt;)  6(&lt;)  7(&lt;)  8(&lt;)  9(&lt;) " /><pippo> xxx </pippo><pippo> yyy </pippo> hello!
<t y=" 0(&gt;)  1(&gt;)  2(&gt;)  3(&gt;)  4(&gt;)  5(&gt;)  6(&gt;)  7(&gt;)  8(&gt;)  9(&gt;) " /><pippo> xxx </pippo><pippo> yyy </pippo> hello!
