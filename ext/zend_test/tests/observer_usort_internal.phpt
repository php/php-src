--TEST--
Observer: native sorting callbacks retain balanced begin and end events
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
--FILE--
<?php
foreach (['strcmp', strcmp(...)] as $callback) {
    $items = ['b', 'a'];
    usort($items, $callback);
    echo $items[0], $items[1], "\n";
}
?>
--EXPECTF--
<!-- init '%s%eobserver_usort_internal.php' -->
<file '%s%eobserver_usort_internal.php'>
  <!-- init usort() -->
  <usort>
    <!-- init strcmp() -->
    <strcmp>
    </strcmp>
  </usort>
ab
  <usort>
    <strcmp>
    </strcmp>
  </usort>
ab
</file '%s%eobserver_usort_internal.php'>
