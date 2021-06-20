--TEST--
Observer: Basic generator observability
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
zend_test.observer.show_return_value=1
--FILE--
<?php
function getResults() {
    for ($i = 10; $i < 13; $i++) {
        yield $i;
    }
}

function doSomething() {
    $generator = getResults();
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
    <!-- init getResults() -->
    <getResults>
    </getResults:10>
10
    <getResults>
    </getResults:11>
11
    <getResults>
    </getResults:12>
12
    <getResults>
    </getResults:NULL>
  </doSomething:'Done'>
Done
</file '%s%eobserver_generator_%d.php'>
