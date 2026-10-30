--TEST--
syntax highlighting
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

$filename = __DIR__."/014.test.php";
$code = '
<?php
$test = "var"; //var
/* test class */
class test {
    private $var = array();

    public static function foo(Test $arg) {
        echo "hello";
        var_dump($this);
    }
}

$o = new test;
?>
';

file_put_contents($filename, $code);

$filename_escaped = escapeshellarg($filename);
var_dump(`$php -n -s $filename_escaped`);
var_dump(`$php -n -s unknown`);

@unlink($filename);

echo "Done\n";
?>
--EXPECT--
string(1158) "<pre><code style="color: #000000">
<span style="color: #0000BB">&lt;?php
$test </span><span style="color: #007700">= </span><span style="color: #DD0000">"var"</span><span style="color: #007700">; </span><span style="color: #FF8000">//var
/* test class */
</span><span style="color: #007700">class </span><span style="color: #0000BB">test </span><span style="color: #007700">{
    private </span><span style="color: #0000BB">$var </span><span style="color: #007700">= array();

    public static function </span><span style="color: #0000BB">foo</span><span style="color: #007700">(</span><span style="color: #0000BB">Test $arg</span><span style="color: #007700">) {
        echo </span><span style="color: #DD0000">"hello"</span><span style="color: #007700">;
        </span><span style="color: #0000BB">var_dump</span><span style="color: #007700">(</span><span style="color: #0000BB">$this</span><span style="color: #007700">);
    }
}

</span><span style="color: #0000BB">$o </span><span style="color: #007700">= new </span><span style="color: #0000BB">test</span><span style="color: #007700">;
</span><span style="color: #0000BB">?&gt;
</span></code></pre>"
Could not open input file: unknown
NULL
Done
