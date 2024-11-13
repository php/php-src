--TEST--
GH-13712 (Segmentation fault for enabled observers after pass 4)
--EXTENSIONS--
opcache
zend_test
--INI--
zend_test.observer.enabled=1
zend_test.observer.show_output=1
zend_test.observer.observe_all=1
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=0x4069
--FILE--
<?php

function inner() {
	echo "Ok\n";
}

function foo() {
	// If stack size is wrong, inner() will corrupt the previous observed frame
	inner();
}

// After foo() def so that we land here, with step_two undone for foo() first
function outer() {
	// Pass 15 does constant string propagation, which gives a ZEND_INIT_DYNAMIC_FCALL on a const which Pass 4 will optimize
	// Pass 4 must calc the right stack size here
	(NAME)();
}

const NAME = "foo";

outer();

?>
--EXPECTF--
<!-- init '%s' -->
<file '%s'>
  <!-- init outer() -->
  <outer>
    <!-- init foo() -->
    <foo>
      <!-- init inner() -->
      <inner>
Ok
      </inner>
    </foo>
  </outer>
</file '%s'>

