<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';

class gtVariationTestCaseMethodTest extends PHPUnit_Framework_TestCase {

  public function testTestCase() {
   
    $f = new gtMethod('DOMDocument','createAttribute');
    $f->setArgumentNames();
    $f->setArgumentLists();

    $f->setConstructorArgumentNames();
    $f->setConstructorInitStatements();
    
    $optSect = new gtOptionalSections();
    
    $vtc = gtVariationTestCase::getInstance($optSect, 'method');
    $vtc->setUp($f, 1, 'int');
    $vtc->constructTestCase();
    $fs = $vtc->toString();
    
    $this->assertTrue(is_string($fs));
   
  }
}
?>