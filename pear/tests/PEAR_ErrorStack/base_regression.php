<?php
// $Revision$
/**
 * Basic regression test for PEAR_ErrorStack::getFileLine()
 * 
 * phpUnit can't test global code properly because of its design, so I designed
 * this instead
 * @package PEAR_ErrorStack
 * @subpackage tests
 * @author Greg Beaver <cellog@php.net>
 */
require_once 'PEAR/ErrorStack.php';
$result = array(
'passed' => array(),
'failed' => array()
);
$stack = &PEAR_ErrorStack::singleton('test');
$testNumber = 1;
// test basic global file/line
$stack->push(3);
$testline = __LINE__ - 1;

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'function' => 'include_once',
      'line' => $testline));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test basic in-function file/line #2
function testfunc() { global $stack, $testline;
$stack->push(3);
$testline = __LINE__ - 1;
}
testfunc();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'testfunc'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test basic in-static method file/line #3
class stclass {
function stfunc() { global $stack, $testline;
$stack->push(3);
$testline = __LINE__ - 1;
}
}
stclass::stfunc();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'stfunc',
      'class' => 'stclass'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test basic in-method file/line #4
class normalclass {
function normalfunc() { global $stack, $testline;
$stack->push(3);
$testline = __LINE__ - 1;
}
}
$z = new normalclass;
$z->normalfunc();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'normalfunc',
      'class' => 'normalclass'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test global eval file/line #5
eval('$stack->push(3);');
$testline = __LINE__ - 1;

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'function' => 'include_once',
      'line' => $testline));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test in-function eval file/line #6
function test2() {
    global $testline, $stack;
eval('$stack->push(3);');
$testline = __LINE__ - 1;
}
test2();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'test2'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test in-static method eval file/line #7
class test3 {
function test3() {
    global $testline, $stack;
eval('$stack->push(3);');
$testline = __LINE__ - 1;
}
}
test3::test3();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'test3',
      'class' => 'test3'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test in-method eval file/line #8
class test4 {
function test4() {
    global $testline, $stack;
eval('$stack->push(3);');
$testline = __LINE__ - 1;
}
}
$z = new test4;
$z->test4();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'test4',
      'class' => 'test4'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test global create_function file/line #9
$a = create_function('', '$GLOBALS["stack"]->push(3);');
$testline = __LINE__ - 1;
$a();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'create_function() code'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test in-function create_function file/line #10
function test7() { global $a;
$a();
}
test7();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'create_function() code'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test in-static method create_function file/line #11
class test8 {
function test8() { global $a;
$a();
}
}
test8::test8();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'create_function() code'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test in-method create_function file/line #12
class test9 {
function test9() { global $a;
$a();
}
}
$z = new test9;
$z->test9();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'create_function() code'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$result['number'] = $testNumber;

$testNumber++;
// test static basic global file/line #13
PEAR_ErrorStack::staticPush('test', 3);
$testline = __LINE__ - 1;

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'function' => 'include_once',
      'line' => $testline));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static basic in-function file/line #14
function testfunc2() { global $stack, $testline;
PEAR_ErrorStack::staticPush('test', 3);
$testline = __LINE__ - 1;
}
testfunc2();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'testfunc2'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static basic in-static method file/line #15
class stclass2 {
function stfunc() { global $stack, $testline;
PEAR_ErrorStack::staticPush('test', 3);
$testline = __LINE__ - 1;
}
}
stclass2::stfunc();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'stfunc',
      'class' => 'stclass2'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static basic in-method file/line #16
class normalclass2 {
function normalfunc() { global $stack, $testline;
PEAR_ErrorStack::staticPush('test', 3);
$testline = __LINE__ - 1;
}
}
$z = new normalclass2;
$z->normalfunc();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'normalfunc',
      'class' => 'normalclass2'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static global eval file/line #17
eval('PEAR_ErrorStack::staticPush(\'test\', 3);');
$testline = __LINE__ - 1;

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'function' => 'include_once',
      'line' => $testline));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static in-function eval file/line #18
function test22() {
    global $testline, $stack;
eval('PEAR_ErrorStack::staticPush("test", 3);');
$testline = __LINE__ - 1;
}
test22();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'test22'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static in-static method eval file/line #19
class test32 {
function test3() {
    global $testline, $stack;
eval('PEAR_ErrorStack::staticPush(\'test\',3);');
$testline = __LINE__ - 1;
}
}
test32::test3();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'test3',
      'class' => 'test32'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static in-method eval file/line #20
class test42 {
function test4() {
    global $testline, $stack;
eval('PEAR_ErrorStack::staticPush(\'test\',3);');
$testline = __LINE__ - 1;
}
}
$z = new test42;
$z->test4();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'test4',
      'class' => 'test42'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static global create_function file/line #21
$a = create_function('', 'PEAR_ErrorStack::staticPush("test",3);');
$testline = __LINE__ - 1;
$a();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'create_function() code'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static in-function create_function file/line #22
function test72() { global $a;
$a();
}
test72();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'create_function() code'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static in-static method create_function file/line #23
class test82 {
function test8() { global $a;
$a();
}
}
test82::test8();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'create_function() code'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$testNumber++;
// test static in-method create_function file/line #24
class test92 {
function test9() { global $a;
$a();
}
}
$z = new test92;
$z->test9();

$ret = $stack->pop();
$diff = array_diff_assoc($ret['context'],
array('file' => __FILE__,
      'line' => $testline,
      'function' => 'create_function() code'));

if ($diff !== array()) {
    $result['failed'][$testNumber] = $diff;
} else {
    $result['passed'][$testNumber] = true;
}

$result['number'] = $testNumber;

return $result;
/**
 * Utility function
 */
function isIncludeable($path)
{
    if (file_exists(realpath($path)) && is_readable(realpath($path))) {
        return true;
    }
    foreach (explode(PATH_SEPARATOR, get_include_path()) as $prepend) {
    	$test = realpath($prepend . DIRECTORY_SEPARATOR . $path);
        if (file_exists($test) && is_readable($test)) {
            return true;
        }
    }
}
/**
 * Mock PHPUnit object
 */
class Mock_PHPUnit {
    var $name;
    function getName()
    {
        return 'base regression test ' . $this->name;
    }
}
?>