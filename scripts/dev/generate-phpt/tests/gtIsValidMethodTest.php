<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';


class gtIsValidMethodTest extends PHPUnit_Framework_TestCase {

  public function testValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'DOMDocument', '-m', 'createAttribute'));
    $ch = new gtIsValidMethod();
    $this->assertTrue($ch->check($clo));
  }

  public function testNotValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'DOMDocument', '-m', 'blah'));
    $ch = new gtIsValidMethod();
    $this->assertFalse($ch->check($clo));
  }
  
  public function testNotGiven() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php','-b'));
    $ch = new gtIsValidMethod();
    $this->assertTrue($ch->check($clo));
  }
  
  public function testMessage() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'blah'));
    $ch = new gtIsvalidMethod();
    $this->assertEquals($ch->getMessage(), gtText::get('unknownMethod'));
  }
}
?>