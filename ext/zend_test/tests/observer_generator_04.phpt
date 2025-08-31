--TEST--
Observer: Generator with manual traversal
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
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
<!-- init '%s' -->
<file '%s'>
  <!-- init doSomething() -->
  <doSomething>
    <!-- init Generator::current() -->
    <Generator::current>
      <!-- init fooResults() -->
      <fooResults>
Starting generator
      </fooResults:0>
    </Generator::current:0>
    <Generator::current>
    </Generator::current:0>
0
    <Generator::current>
    </Generator::current:0>
    <!-- init Generator::next() -->
    <Generator::next>
      <fooResults>
      </fooResults:1>
    </Generator::next:NULL>
    <Generator::current>
    </Generator::current:1>
    <Generator::current>
    </Generator::current:1>
1
    <Generator::current>
    </Generator::current:1>
    <Generator::next>
      <fooResults>
      </fooResults:2>
    </Generator::next:NULL>
    <Generator::current>
    </Generator::current:2>
    <Generator::current>
    </Generator::current:2>
2
    <Generator::current>
    </Generator::current:2>
    <Generator::next>
      <fooResults>
      </fooResults:3>
    </Generator::next:NULL>
    <Generator::current>
    </Generator::current:3>
    <Generator::current>
    </Generator::current:3>
3
    <Generator::current>
    </Generator::current:3>
    <Generator::next>
      <fooResults>
      </fooResults:4>
    </Generator::next:NULL>
    <Generator::current>
    </Generator::current:4>
    <Generator::current>
    </Generator::current:4>
4
    <Generator::current>
    </Generator::current:4>
    <Generator::next>
      <fooResults>
      </fooResults:5>
    </Generator::next:NULL>
    <Generator::current>
    </Generator::current:5>
    <Generator::current>
    </Generator::current:5>
5
    <Generator::current>
    </Generator::current:5>
    <!-- init Generator::send() -->
    <Generator::send>
      <fooResults>
      </fooResults:NULL>
    </Generator::send:NULL>
    <Generator::next>
    </Generator::next:NULL>
    <Generator::current>
    </Generator::current:NULL>
  </doSomething:'Done'>
Done
</file '%s'>
