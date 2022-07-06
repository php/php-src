--TEST--
Observer: Generator with manual traversal
--SKIPIF--
<?php if (!extension_loaded('zend-test')) die('skip: zend-test extension required'); ?>
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function fooResults() {
    echo 'Starting generator' . PHP_EOL;
    $i = 0;
    while (true) {
        if (yield $i++) break;
    }
}

function doSomething() {
    $generator = fooResults();

    while($generator->current() !== NULL) {
        echo $generator->current() . PHP_EOL;
        if ($generator->current() === 5) {
            $generator->send('Boop');
        }
        $generator->next();
    }

    return 'Done';
}

echo doSomething() . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s%eobserver_generator_%d.php' -->
<file '%s%eobserver_generator_%d.php'>
  <!-- init doSomething() -->
  <doSomething>
    <!-- init fooResults() -->
    <fooResults>
Starting generator
    </fooResults:0>
0
    <fooResults>
    </fooResults:1>
1
    <fooResults>
    </fooResults:2>
2
    <fooResults>
    </fooResults:3>
3
    <fooResults>
    </fooResults:4>
4
    <fooResults>
    </fooResults:5>
5
    <fooResults>
    </fooResults:NULL>
  </doSomething:'Done'>
Done
</file '%s%eobserver_generator_%d.php'>
