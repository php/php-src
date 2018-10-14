<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';

class gtBasicTestCaseMethodTest extends PHPUnit_Framework_TestCase {


  public function testTestCase() {

    $f = new gtMethod('DOMDocument','createAttribute');
    $f->setArgumentNames();
    $f->setArgumentLists();
    $f->setInitialisationStatements();
    $f->setConstructorArgumentNames();
    $f->setConstructorInitStatements();

    $optSect = new gtOptionalSections();
    $btc = gtBasicTestCaseMethod::getInstance($optSect, 'method');
    $btc->setMethod($f);
    $btc->constructTestCase();


    $fs = $btc->toString();
    $this->assertTrue(is_string($fs));

  }
}
?>
