<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';


class gtIsSpecifiedTestTypeTest extends PHPUnit_Framework_TestCase {

  public function testValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'DOMDocument','-b'));
    $ch = new gtIsSpecifiedTestType();
    $this->assertTrue($ch->check($clo));
  }

  public function testNotValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'DOMDocument'));
    $ch = new gtIsSpecifiedTestType();
    $this->assertFalse($ch->check($clo));
  }

  public function testMessage() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'blah'));
    $ch = new gtIsSpecifiedtestType();
    $this->assertEquals($ch->getMessage(), gtText::get('testTypeNotSpecified'));
  }
}
?>
