<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';


class gtCodeSnippetTest extends PHPUnit_Framework_TestCase
{
  
  public function testAppend() {
    $array = array('something', 'nothing');
    $array = gtCodeSnippet::append('loopClose', $array);
    $this->assertEquals($array[2], '}');
  }
}
?>