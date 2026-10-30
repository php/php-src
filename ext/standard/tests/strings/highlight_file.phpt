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

$filename = __DIR__."/highlight_file.dat";

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
Deprecated: Directive 'allow_url_include' is deprecated in Unknown on line 0

Warning: highlight_file(%shighlight_file.dat): Failed to open stream: No such file or directory in %s on line %d

Warning: highlight_file(): Failed opening '%shighlight_file.dat' for highlighting in %s on line %d
bool(false)
<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php </span><span style="color: #007700">echo </span><span style="color: #DD0000">"test"</span><span style="color: #007700">; </span><span style="color: #0000BB">?&gt;</span></code></pre>bool(true)
<pre><code style="color: #000000"><span style="color: #0000BB">&lt;?php </span><span style="color: #007700">echo </span><span style="color: #DD0000">"test ?&gt;</span></code></pre>bool(true)
<pre><code style="color: #000000">
<span style="color: #0000BB">&lt;?php
</span><span style="color: #007700">class </span><span style="color: #0000BB">test </span><span style="color: #007700">{
    public </span><span style="color: #0000BB">$var </span><span style="color: #007700">= </span><span style="color: #0000BB">1</span><span style="color: #007700">;
    private function </span><span style="color: #0000BB">foo</span><span style="color: #007700">() { echo </span><span style="color: #DD0000">"foo"</span><span style="color: #007700">; }
    public function </span><span style="color: #0000BB">bar</span><span style="color: #007700">() { </span><span style="color: #0000BB">var_dump</span><span style="color: #007700">(</span><span style="color: #0000BB">test</span><span style="color: #007700">::</span><span style="color: #0000BB">foo</span><span style="color: #007700">()); }
}
</span><span style="color: #0000BB">?&gt;</span></code></pre>bool(true)
Done
