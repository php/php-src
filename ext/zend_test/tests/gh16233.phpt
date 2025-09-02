--TEST--
GH-16233 (Observer segfault when calling user function in internal function via trampoline)
--EXTENSIONS--
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
--FILE--
<?php

function callee() {
    echo "in callee\n";
}

$test = new _ZendTestMagicCallForward;
$test->callee();
echo "done\n";

?>
--EXPECTF--
<!-- init '%sgh16233.php' -->
<file '%sgh16233.php'>
  <!-- init _ZendTestMagicCallForward::__call() -->
  <_ZendTestMagicCallForward::__call>
    <!-- init callee() -->
    <callee>
in callee
    </callee>
  </_ZendTestMagicCallForward::__call>
done
</file '%sgh16233.php'>
