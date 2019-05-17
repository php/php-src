--TEST--
Bug #24951 (ob_flush() destroys output handler)
--FILE--
<?php
function test($s, $mode)
{
	return (($mode & PHP_OUTPUT_HANDLER_START)?"[":"") . $s . (($mode & PHP_OUTPUT_HANDLER_END)?"]\n":"");
}
function t1()
{
	ob_start("test");
	echo "Hello from t1 1 ";
        echo "Hello from t1 2 ";
        ob_end_flush();
}
function t2()
{
	ob_start("test");
	echo "Hello from t2 1 ";
        ob_flush();
        echo "Hello from t2 2 ";
        ob_end_flush();
}
function t3()
{
	ob_start("test");
        echo "Hello from t3 1 ";
        ob_clean();
        echo "Hello from t3 2 ";
        ob_end_flush();
}

t1(); echo "\n";
t2(); echo "\n";
t3(); echo "\n";
?>
--EXPECT--
[Hello from t1 1 Hello from t1 2 ]

[Hello from t2 1 Hello from t2 2 ]

Hello from t3 2 ]
