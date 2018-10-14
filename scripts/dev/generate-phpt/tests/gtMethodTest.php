<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';


class gtMethodTest extends PHPUnit_Framework_TestCase
{
  public function testGetParams() {
    $m = new gtMethod('DOMDocument', 'createAttribute');
    $m->setArgumentNames();
    $a = $m->getMandatoryArgumentNames();
    $this->assertEquals($a[0], 'name');
  }

  public function testConstructor() {
    $m = new gtMethod('DOMDocument', 'createAttribute');
    $m->setConstructorArgumentNames();
    $a = $m->getConstructorArgumentNames();
    $this->assertEquals($a[0], 'version');
    $this->assertEquals($a[1], 'encoding');
  }

  public function testExtraParamList() {
    $m = new gtMethod('DOMDocument', 'createAttribute');
    $m->setArgumentNames();
    $m->setExtraArgumentList();
    $this->assertEquals('$name, $extra_arg',$m->getExtraArgumentList());
  }

  public function testShortParamList() {
    $m = new gtMethod('DOMDocument', 'createAttribute');
    $m->setArgumentNames();
    $m->setShortArgumentList();
    $this->assertEquals('',$m->getShortArgumentList());
  }

  public function testAllParamList() {
    $m = new gtMethod('DOMDocument', 'createAttribute');
    $m->setArgumentNames();
    $m->setValidArgumentLists();
    $a = $m->getValidArgumentLists();
    $this->assertEquals('$name',$a[0]);
  }

  public function testMaxParamList() {
    $m = new gtMethod('DOMDocument', 'createAttribute');
    $m->setArgumentNames();
    $m->setValidArgumentLists();
    $this->assertEquals('$name',$m->getMaximumArgumentList());
  }



  public function testConstructorList() {
    $m = new gtMethod('Phar', 'buildFromDirectory');
    $m->setArgumentNames();
    $m->setConstructorArgumentNames();

    $m->setConstructorArgumentList();
    $this->assertEquals('$filename, $flags, $alias, $fileformat',$m->getConstructorArgumentList());

  }

  public function testConstructorInit() {
    $m = new gtMethod('Phar', 'buildFromDirectory');
    $m->setArgumentNames();
    $m->setConstructorArgumentNames();

    $m->setConstructorInitStatements();
    $a = $m->getConstructorInitStatements();
    $this->assertEquals('$filename = ',$a[0]);
    $this->assertEquals('$flags = ',$a[1]);
    $this->assertEquals('$alias = ',$a[2]);
    $this->assertEquals('$fileformat = ',$a[3]);
  }




}

?>
