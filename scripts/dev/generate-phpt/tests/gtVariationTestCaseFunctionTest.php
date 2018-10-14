<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';

class gtVariationTestCaseFunctionTest extends PHPUnit_Framework_TestCase {

  public function testTestCase() {

    $f = new gtFunction('cos');
    $f->setArgumentNames();
    $f->setArgumentLists();

    $optSect = new gtOptionalSections();

    $vtc = gtVariationTestCase::getInstance($optSect);
    $vtc->setUp($f, 1, 'int');
    $vtc->constructTestCase();

    $fs = $vtc->toString();
    $this->assertTrue(is_string($fs));

  }

  public function testTestCase2() {

    $f = new gtFunction('date_sunrise');
    $f->setArgumentNames();
    $f->setArgumentLists();
    $a = $f->getMandatoryArgumentNames();

    $optSect = new gtOptionalSections();

    $vtc = gtVariationTestCase::getInstance($optSect);
    $vtc->setUp($f, 6, 'int');
    $vtc->constructTestCase();

    $fs = $vtc->toString();
    $this->assertTrue(is_string($fs));

  }

  public function testTestCase3() {

    $f = new gtFunction('date_sunrise');
    $f->setArgumentNames();
    $f->setArgumentLists();

    $optSect = new gtOptionalSections();

    $vtc = gtVariationTestCase::getInstance($optSect);
    $vtc->setUp($f, 6, 'array');
    $vtc->constructTestCase();

    $fs = $vtc->toString();
    $this->assertTrue(is_string($fs));

  }
}
?>
