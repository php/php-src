<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';


class gtIsSpecifiedFunctionOrMethodTest extends PHPUnit_Framework_TestCase {

  public function testValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-m', 'blah'));
    $ch = new gtIsSpecifiedFunctionOrMethod();
    $this->assertTrue($ch->check($clo));
  }

  public function testValid2() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-f', 'blah'));
    $ch = new gtIsSpecifiedFunctionOrMethod();
    $this->assertTrue($ch->check($clo));
  }

   public function testNotValid() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-b'));
    $ch = new gtIsSpecifiedFunctionOrMethod();
    $this->assertFalse($ch->check($clo));

  }

  public function testMessage() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-c', 'blah'));
    $ch = new gtIsSpecifiedFunctionOrMethod();
    $this->assertEquals($ch->getMessage(), gtText::get('functionOrMethodNotSpecified'));
  }
}
?>
