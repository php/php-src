--TEST--
Verbose  tidy_getopt()
--EXTENSIONS--
tidy
--INI--
tidy.default_config=
--FILE--
<?php
$a = new tidy(__DIR__."/007.html");
echo "Current Value of 'tidy-mark': ";
var_dump($a->getopt("tidy-mark"));
echo "Current Value of 'error-file': ";
var_dump($a->getopt("error-file"));
echo "Current Value of 'tab-size': ";
var_dump($a->getopt("tab-size"));

try {
    $a->getopt('bogus-opt');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    tidy_getopt($a, 'non-ASCII string ���');
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
Current Value of 'tidy-mark': bool(false)
Current Value of 'error-file': string(0) ""
Current Value of 'tab-size': int(8)
tidy::getOpt(): Argument #1 ($option) is an invalid configuration option, "bogus-opt" given
tidy_getopt(): Argument #2 ($option) is an invalid configuration option, "non-ASCII string ���" given
