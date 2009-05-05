--TEST--
highlight_file() tests
--INI--
highlight.string=#DD0000
highlight.comment=#FF9900
highlight.keyword=#007700
highlight.default=#0000BB
highlight.html=#000000
allow_url_include=1
allow_url_fopen=1
--FILE--
<?php

$filename = dirname(__FILE__)."/highlight_file.dat";

var_dump(highlight_file());
var_dump(highlight_file($filename));

var_dump(highlight_file('data:,<?php echo "test"; ?>'));

var_dump(highlight_file('data:,<?php echo "test ?>'));

$data = '
<?php 
 class test { 
	 public $var = 1; 
	 private function foo() { echo "foo"; }
	 public function bar() { var_dump(test::foo()); }
 }  
?>';

file_put_contents($filename, $data);
var_dump(highlight_file($filename));


@unlink($filename);
echo "Done\n";
?>
--EXPECTF--
Warning: highlight_file() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

Warning: highlight_file(%shighlight_file.dat): failed to open stream: No such file or directory in %s on line %d

Warning: highlight_file(): Failed opening '%shighlight_file.dat' for highlighting in %s on line %d
bool(false)
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php&nbsp;</span><span style="color: #007700">echo&nbsp;</span><span style="color: #DD0000">"test"</span><span style="color: #007700">;&nbsp;</span><span style="color: #0000BB">?&gt;</span>
</span>
</code>bool(true)
<code><span style="color: #000000">
<span style="color: #0000BB">&lt;?php&nbsp;</span><span style="color: #007700">echo&nbsp;</span><span style="color: #DD0000">"test&nbsp;?&gt;</span>
</span>
</code>bool(true)
<code><span style="color: #000000">
<br /><span style="color: #0000BB">&lt;?php&nbsp;<br />&nbsp;</span><span style="color: #007700">class&nbsp;</span><span style="color: #0000BB">test&nbsp;</span><span style="color: #007700">{&nbsp;<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;public&nbsp;</span><span style="color: #0000BB">$var&nbsp;</span><span style="color: #007700">=&nbsp;</span><span style="color: #0000BB">1</span><span style="color: #007700">;&nbsp;<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;private&nbsp;function&nbsp;</span><span style="color: #0000BB">foo</span><span style="color: #007700">()&nbsp;{&nbsp;echo&nbsp;</span><span style="color: #DD0000">"foo"</span><span style="color: #007700">;&nbsp;}<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;public&nbsp;function&nbsp;</span><span style="color: #0000BB">bar</span><span style="color: #007700">()&nbsp;{&nbsp;</span><span style="color: #0000BB">var_dump</span><span style="color: #007700">(</span><span style="color: #0000BB">test</span><span style="color: #007700">::</span><span style="color: #0000BB">foo</span><span style="color: #007700">());&nbsp;}<br />&nbsp;}&nbsp;&nbsp;<br /></span><span style="color: #0000BB">?&gt;</span>
</span>
</code>bool(true)
Done
