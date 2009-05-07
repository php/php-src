<?php
require_once 'PHPUnit/Framework.php';
  require_once dirname(__FILE__) . '/../src/gtAutoload.php';


  class gtCommandLineOptionsTest extends PHPUnit_Framework_TestCase
  {
    
    /**
    * @expectedException RuntimeException
    */
    public function testNoOption() {
      $clo = new gtCommandLineOptions();
      $clo->parse(array('generate-phpt.php'));
    }

    public function testShortOption() {
      $clo = new gtCommandLineOptions();
      $clo->parse(array('generate-phpt.php', '-h'));
      $this->assertTrue($clo->hasOption('h'));
    }

    public function testShortOptionArgument() {
      $clo = new gtCommandLineOptions();
      $clo->parse(array('generate-phpt.php', '-f', 'some-function'));
      $this->assertTrue($clo->hasOption('f'));
      $this->assertEquals('some-function', $clo->getOption('f'));
    }
    
    /**
    * @expectedException RuntimeException
    */
      public function testInvalidOption() {
      $clo = new gtCommandLineOptions();
      $clo->parse(array('generate-phpt.php', '-z'));
    }
    
    /**
    * @expectedException RuntimeException
    */
   public function testMissingArgument() {
      $clo = new gtCommandLineOptions();
      $clo->parse(array('generate-phpt.php', '-f'));
    }
  }
?>