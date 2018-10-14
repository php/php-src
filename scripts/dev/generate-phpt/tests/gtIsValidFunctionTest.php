<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';


class gtIsValidFunctionTest extends PHPUnit_Framework_TestCase {

  public function testValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-f', 'cos'));
    $ch = new gtIsValidFunction();
    $this->assertTrue($ch->check($clo));
  }

  public function testNotValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-f', 'blah'));
    $ch = new gtIsValidFunction();
    $this->assertFalse($ch->check($clo));
  }

  public function testNotSupplied() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php','-b'));
    $ch = new gtIsValidFunction();
    $this->assertTrue($ch->check($clo));
  }

  public function testMessage() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'blah'));
    $ch = new gtIsvalidFunction();
    $this->assertEquals($ch->getMessage(), gtText::get('unknownFunction'));
  }
}
?>
