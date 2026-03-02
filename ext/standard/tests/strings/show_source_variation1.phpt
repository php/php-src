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
--EXPECT--
*** Test by calling method or function with its expected arguments and php output ***
baz
<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php
</span><span style="color: #007700">echo </span><span style="color: #DD0000">"*** Test by calling method or function with its expected arguments and php output ***\n"</span><span style="color: #007700">;
</span><span style="color: #0000BB">$foo </span><span style="color: #007700">= </span><span style="color: #DD0000">'bar'</span><span style="color: #007700">;
</span><span style="color: #0000BB">$baz </span><span style="color: #007700">= </span><span style="color: #DD0000">"something "</span><span style="color: #007700">.</span><span style="color: #0000BB">$foo</span><span style="color: #007700">.</span><span style="color: #DD0000">"\n"</span><span style="color: #007700">;

if ( </span><span style="color: #0000BB">$foo </span><span style="color: #007700">== </span><span style="color: #DD0000">'bar' </span><span style="color: #007700">)
{
  </span><span style="color: #0000BB">$baz </span><span style="color: #007700">= </span><span style="color: #DD0000">"baz\n"</span><span style="color: #007700">;
}

 </span><span style="color: #FF8000">/* some code here */
</span><span style="color: #007700">echo </span><span style="color: #0000BB">$baz</span><span style="color: #007700">;
</span><span style="color: #0000BB">show_source</span><span style="color: #007700">(</span><span style="color: #0000BB">__FILE__</span><span style="color: #007700">);
echo </span><span style="color: #0000BB">$foo</span><span style="color: #007700">;
</span><span style="color: #0000BB">?&gt;
</span></code></pre>bar
