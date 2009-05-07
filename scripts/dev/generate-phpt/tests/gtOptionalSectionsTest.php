<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';


class gtOptionalSectionsTest extends PHPUnit_Framework_TestCase
{
  public function testBasic() {

    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-s', 'skipif:ini'));

    $opt = new gtOptionalSections();
    $opt->setOptions($clo);
    $a = $opt->getOptions();
    $this->assertEquals(true, $a['skipif']);
    $this->assertEquals(true, $a['ini']);
    $this->assertEquals(false, $a['clean']);
  }

  /**
   * @expectedException RuntimeException
   */
  public function testException() {
    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-s', 'blah'));
    $opt = new gtOptionalSections();
    $opt->setOptions($clo);
  }
  
  public function testSkip() {
    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-s', 'skipif', '-x', 'standard'));
    $opt = new gtOptionalSections();
    $opt->setOptions($clo);
    
    $opt = new gtOptionalSections();
    $opt->setOptions($clo);
    
    $this->assertEquals('standard', $opt->getSkipifExt() );

  }
  
  public function testSkipKey() {
    $clo = new gtCommandLineOptions();
    $clo->parse(array('generate-phpt.php', '-s', 'skipif', '-k', 'win'));
    $opt = new gtOptionalSections();
    $opt->setOptions($clo);
    
    $opt = new gtOptionalSections();
    $opt->setOptions($clo);
    
    $this->assertEquals('win', $opt->getSkipifKey() );

  }

}
?>