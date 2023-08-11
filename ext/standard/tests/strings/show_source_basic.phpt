--TEST--
Test function show_source() by calling it with its expected arguments, more test for highlight_file()
--CREDITS--
Francesco Fullone ff@ideato.it
#PHPTestFest Cesena Italia on 2009-06-20
--FILE--
<?php
echo "*** Test by calling method or function with its expected arguments ***\n";
$foo = 'bar';
$baz = "something ".$foo."\n";

if ( $foo == 'bar' )
{
  $baz = 'baz';
}

 /* some code here */

show_source(__FILE__);

?>
--EXPECT--
*** Test by calling method or function with its expected arguments ***
<pre style="color: #000000"><code style="color: #0000BB">&lt;?php
</code><code style="color: #007700">echo </code><code style="color: #DD0000">"*** Test by calling method or function with its expected arguments ***\n"</code><code style="color: #007700">;
</code><code style="color: #0000BB">$foo </code><code style="color: #007700">= </code><code style="color: #DD0000">'bar'</code><code style="color: #007700">;
</code><code style="color: #0000BB">$baz </code><code style="color: #007700">= </code><code style="color: #DD0000">"something "</code><code style="color: #007700">.</code><code style="color: #0000BB">$foo</code><code style="color: #007700">.</code><code style="color: #DD0000">"\n"</code><code style="color: #007700">;

if ( </code><code style="color: #0000BB">$foo </code><code style="color: #007700">== </code><code style="color: #DD0000">'bar' </code><code style="color: #007700">)
{
  </code><code style="color: #0000BB">$baz </code><code style="color: #007700">= </code><code style="color: #DD0000">'baz'</code><code style="color: #007700">;
}

 </code><code style="color: #FF8000">/* some code here */

</code><code style="color: #0000BB">show_source</code><code style="color: #007700">(</code><code style="color: #0000BB">__FILE__</code><code style="color: #007700">);

</code><code style="color: #0000BB">?&gt;
</code></pre>
