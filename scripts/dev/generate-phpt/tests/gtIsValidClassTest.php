<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';


class gtIsValidClassTest extends PHPUnit_Framework_TestCase {

  public function testValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'DOMDocument'));
    $ch = new gtIsValidClass();
    $this->assertTrue($ch->check($clo));
  }

  public function testNotValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'blah'));
    $ch = new gtIsValidClass();
    $this->assertFalse($ch->check($clo));
  }

  public function testNotGiven() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php','-b'));
    $ch = new gtIsValidClass();
    $this->assertTrue($ch->check($clo));
  }

  public function testMessage() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'blah'));
    $ch = new gtIsvalidClass();
    $this->assertEquals($ch->getMessage(), gtText::get('unknownClass'));
  }
}

?>
