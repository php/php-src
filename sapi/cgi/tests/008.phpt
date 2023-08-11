--TEST--
syntax highlighting
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

$filename = __DIR__."/008.test.php";
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

var_dump(`"$php" -n -s "$filename"`);
var_dump(`"$php" -n -s "unknown"`);

@unlink($filename);

echo "Done\n";
?>
--EXPECTF--
string(%d) "X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

<pre style="color: #000000">
<code style="color: #0000BB">&lt;?php
$test </code><code style="color: #007700">= </code><code style="color: #DD0000">"var"</code><code style="color: #007700">; </code><code style="color: #FF8000">//var
/* test class */
</code><code style="color: #007700">class </code><code style="color: #0000BB">test </code><code style="color: #007700">{
    private </code><code style="color: #0000BB">$var </code><code style="color: #007700">= array();

    public static function </code><code style="color: #0000BB">foo</code><code style="color: #007700">(</code><code style="color: #0000BB">Test $arg</code><code style="color: #007700">) {
        echo </code><code style="color: #DD0000">"hello"</code><code style="color: #007700">;
        </code><code style="color: #0000BB">var_dump</code><code style="color: #007700">(</code><code style="color: #0000BB">$this</code><code style="color: #007700">);
    }
}

</code><code style="color: #0000BB">$o </code><code style="color: #007700">= new </code><code style="color: #0000BB">test</code><code style="color: #007700">;
</code><code style="color: #0000BB">?&gt;
</code></pre>"
string(%d) "Status: 404 Not Found
X-Powered-By: PHP/%s
Content-type: text/html%r; charset=.*|%r

No input file specified.
"
Done
