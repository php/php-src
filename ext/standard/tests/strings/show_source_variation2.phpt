--TEST--
Test function show_source() by calling it with its expected arguments and output to variable, more test for highlight_file()
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
echo "*** Test by calling method or function with its expected arguments and output to variable ***\n";
$foo = 'bar';
$baz = "something ".$foo."\n";

if ( $foo == 'bar' )
{
  $baz = "baz\n";
}

 /* some code here */
$source = show_source(__FILE__, true);

var_dump($source);
?>
--EXPECT--
*** Test by calling method or function with its expected arguments and output to variable ***
string(1705) "<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php
</span><span style="color: #007700">echo </span><span style="color: #DD0000">"*** Test by calling method or function with its expected arguments and output to variable ***\n"</span><span style="color: #007700">;
</span><span style="color: #0000BB">$foo </span><span style="color: #007700">= </span><span style="color: #DD0000">'bar'</span><span style="color: #007700">;
</span><span style="color: #0000BB">$baz </span><span style="color: #007700">= </span><span style="color: #DD0000">"something "</span><span style="color: #007700">.</span><span style="color: #0000BB">$foo</span><span style="color: #007700">.</span><span style="color: #DD0000">"\n"</span><span style="color: #007700">;

if ( </span><span style="color: #0000BB">$foo </span><span style="color: #007700">== </span><span style="color: #DD0000">'bar' </span><span style="color: #007700">)
{
  </span><span style="color: #0000BB">$baz </span><span style="color: #007700">= </span><span style="color: #DD0000">"baz\n"</span><span style="color: #007700">;
}

 </span><span style="color: #FF8000">/* some code here */
</span><span style="color: #0000BB">$source </span><span style="color: #007700">= </span><span style="color: #0000BB">show_source</span><span style="color: #007700">(</span><span style="color: #0000BB">__FILE__</span><span style="color: #007700">, </span><span style="color: #0000BB">true</span><span style="color: #007700">);

</span><span style="color: #0000BB">var_dump</span><span style="color: #007700">(</span><span style="color: #0000BB">$source</span><span style="color: #007700">);
</span><span style="color: #0000BB">?&gt;
</span></code></pre>"
