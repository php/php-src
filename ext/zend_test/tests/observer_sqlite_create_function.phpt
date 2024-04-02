--TEST--
Observer: PDO::sqliteCreateFunction() can be observed
--EXTENSIONS--
zend_test
pdo
pdo_sqlite
--INI--
zend_test.observer.enabled=1
zend_test.observer.observe_all=1
--FILE--
<?php

function returnOne() {
    return 1;
}

$db = new PDO('sqlite::memory:');
$db->sqliteCreateFunction('returnOne', 'returnOne', 0);

foreach ($db->query('SELECT returnOne()') as $row) {
    var_dump($row);
}

echo 'Done' . PHP_EOL;
?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init PDO::__construct() -->
  <PDO::__construct>
  </PDO::__construct>
  <!-- init PDO::sqliteCreateFunction() -->
  <PDO::sqliteCreateFunction>
  </PDO::sqliteCreateFunction>
  <!-- init PDO::query() -->
  <PDO::query>
    <!-- init returnOne() -->
    <returnOne>
    </returnOne>
  </PDO::query>
  <!-- init var_dump() -->
  <var_dump>
array(2) {
  ["returnOne()"]=>
  int(1)
  [0]=>
  int(1)
}
  </var_dump>
Done
</file '%s'>
