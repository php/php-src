--TEST--
Observer: Generator with 'yield from'
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function barResults() {
    for ($i = 10; $i < 13; $i++) {
        yield $i;
    }
}

function fooResults() {
    yield 0;
    yield 1;
    yield from barResults();
    yield 42;
}

function doSomething() {
    $generator = fooResults();
    foreach ($generator as $value) {
        echo $value . PHP_EOL;
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
    </fooResults:0>
0
    <fooResults>
    </fooResults:1>
1
    <fooResults>
    </fooResults:1>
    <!-- init barResults() -->
    <barResults>
    </barResults:10>
10
    <barResults>
    </barResults:11>
11
    <barResults>
    </barResults:12>
12
    <barResults>
    </barResults:NULL>
    <fooResults>
    </fooResults:42>
42
    <fooResults>
    </fooResults:NULL>
  </doSomething:'Done'>
Done
</file '%s%eobserver_generator_%d.php'>
