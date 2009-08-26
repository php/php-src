--TEST--
Test function show_source() by calling it with its expected arguments and php output, more test for highlight_file()
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
echo "*** Test by calling method or function with its expected arguments and php output ***\n";
$foo = 'bar';
$baz = "something ".$foo."\n";

if ( $foo == 'bar' ) 
{
  $baz = "baz\n";
}

 /* some code here */
echo $baz;   
show_source(__FILE__);
echo $foo;
?>
--EXPECTF--
*** Test by calling method or function with its expected arguments and php output ***
baz
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php<br /></span><span style="color: #007700">echo&nbsp;</span><span style="color: #DD0000">"***&nbsp;Test&nbsp;by&nbsp;calling&nbsp;method&nbsp;or&nbsp;function&nbsp;with&nbsp;its&nbsp;expected&nbsp;arguments&nbsp;and&nbsp;php&nbsp;output&nbsp;***\n"</span><span style="color: #007700">;<br /></span><span style="color: #0000BB">$foo&nbsp;</span><span style="color: #007700">=&nbsp;</span><span style="color: #DD0000">'bar'</span><span style="color: #007700">;<br /></span><span style="color: #0000BB">$baz&nbsp;</span><span style="color: #007700">=&nbsp;</span><span style="color: #DD0000">"something&nbsp;"</span><span style="color: #007700">.</span><span style="color: #0000BB">$foo</span><span style="color: #007700">.</span><span style="color: #DD0000">"\n"</span><span style="color: #007700">;<br /><br />if&nbsp;(&nbsp;</span><span style="color: #0000BB">$foo&nbsp;</span><span style="color: #007700">==&nbsp;</span><span style="color: #DD0000">'bar'&nbsp;</span><span style="color: #007700">)&nbsp;<br />{<br />&nbsp;&nbsp;</span><span style="color: #0000BB">$baz&nbsp;</span><span style="color: #007700">=&nbsp;</span><span style="color: #DD0000">"baz\n"</span><span style="color: #007700">;<br />}<br /><br />&nbsp;</span><span style="color: #FF8000">/*&nbsp;some&nbsp;code&nbsp;here&nbsp;*/<br /></span><span style="color: #007700">echo&nbsp;</span><span style="color: #0000BB">$baz</span><span style="color: #007700">;&nbsp;&nbsp;&nbsp;<br /></span><span style="color: #0000BB">show_source</span><span style="color: #007700">(</span><span style="color: #0000BB">__FILE__</span><span style="color: #007700">);<br />echo&nbsp;</span><span style="color: #0000BB">$foo</span><span style="color: #007700">;<br /></span><span style="color: #0000BB">?&gt;<br /></span>
</span>
</code>bar
