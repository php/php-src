--TEST--
PEAR_Common::buildProvidesArray test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
if (!function_exists('token_get_all')) {
    echo 'skip';
}
?>
--FILE--
<?php

require_once "PEAR/Common.php";

$testdir = dirname(__FILE__) . DIRECTORY_SEPARATOR . 'pear_common_buildProvidesArraytest';
mkdir($testdir);

$test5 = '
<?php
function test()
{
}

if (trytofool) {
    function fool()
    {
    }
}
class test2 {
    function test2() {
        parent::unused();
        Greg::classes();
        $a = new Pierre;
    }
}

class blah extends test2 {
    /**
     * @nodep Stig
     */
    function blah() 
    {
        Stig::rules();
    }
}
?>
';
$fp = fopen($testdir . DIRECTORY_SEPARATOR . 'test5.php', 'w');
fwrite($fp, $test5);
fclose($fp);

$ret = PEAR_Common::analyzeSourceCode($testdir . DIRECTORY_SEPARATOR . 'test5.php');
echo "pre-test: returns false with valid PHP? ";
echo $ret ? "no\n" : "yes\n";
var_dump($ret);
unlink($testdir . DIRECTORY_SEPARATOR . 'test5.php');
$common = new PEAR_Common;
$common->buildProvidesArray($ret);
var_dump($common->pkginfo);
rmdir($testdir);
?>
--GET--
--POST--
--EXPECT--
pre-test: returns false with valid PHP? no
array(5) {
  ["declared_classes"]=>
  array(2) {
    [0]=>
    string(5) "test2"
    [1]=>
    string(4) "blah"
  }
  ["declared_methods"]=>
  array(2) {
    ["test2"]=>
    array(1) {
      [0]=>
      string(5) "test2"
    }
    ["blah"]=>
    array(1) {
      [0]=>
      string(4) "blah"
    }
  }
  ["declared_functions"]=>
  array(2) {
    [0]=>
    string(4) "test"
    [1]=>
    string(4) "fool"
  }
  ["used_classes"]=>
  array(2) {
    [0]=>
    string(4) "Greg"
    [1]=>
    string(6) "Pierre"
  }
  ["inheritance"]=>
  array(1) {
    ["blah"]=>
    string(5) "test2"
  }
}
array(1) {
  ["provides"]=>
  array(4) {
    ["class;test2"]=>
    array(2) {
      ["type"]=>
      string(5) "class"
      ["name"]=>
      string(5) "test2"
    }
    ["class;blah"]=>
    array(3) {
      ["type"]=>
      string(5) "class"
      ["name"]=>
      string(4) "blah"
      ["extends"]=>
      string(5) "test2"
    }
    ["function;test"]=>
    array(2) {
      ["type"]=>
      string(8) "function"
      ["name"]=>
      string(4) "test"
    }
    ["function;fool"]=>
    array(2) {
      ["type"]=>
      string(8) "function"
      ["name"]=>
      string(4) "fool"
    }
  }
}