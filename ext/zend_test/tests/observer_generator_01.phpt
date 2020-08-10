--TEST--
Observer: Basic generator observability
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
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
<!-- init '%s/observer_generator_01.php' -->
<file '%s/observer_generator_01.php'>
  <!-- init doSomething() -->
  <doSomething>
    <!-- init getResults() -->
    <getResults>
    </getResults>
10
    <getResults>
    </getResults>
11
    <getResults>
    </getResults>
12
    <getResults>
    </getResults>
  </doSomething>
Done
</file '%s/observer_generator_01.php'>
