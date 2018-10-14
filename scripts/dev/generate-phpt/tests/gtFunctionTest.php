cd
<?php
require_once 'PHPUnit/Framework.php';
require_once dirname(__FILE__) . '/../src/gtAutoload.php';


class gtFunctionTest extends PHPUnit_Framework_TestCase
{
  public function testArguments() {

    $f = new gtFunction('cos');
    $f->setArgumentNames();
    $m = $f->getMandatoryArgumentNames();
    $this->assertEquals($m[0], 'number');
  }

  public function testArguments2() {

    $f = new gtFunction('version_compare');
    $f->setArgumentNames();
    $m = $f->getMandatoryArgumentNames();
    $o = $f->getOptionalArgumentNames();
    $this->assertEquals($m[0], 'ver1');
    $this->assertEquals($m[1], 'ver2');
    $this->assertEquals($o[0], 'oper');

  }

  public function testExtraArguments() {

    $f = new gtFunction('version_compare');
    $f->setArgumentNames();
    $f->setExtraArgumentList();

    $this->assertEquals('$ver1, $ver2, $oper, $extra_arg', $f->getExtraArgumentList());
  }

  public function testShortArguments() {

    $f = new gtFunction('version_compare');
    $f->setArgumentNames();
    $f->setShortArgumentList();

    $this->assertEquals('$ver1', $f->getShortArgumentList());
  }

   public function testAllArgumentList() {

    $f = new gtFunction('version_compare');
    $f->setArgumentNames();
    $f->setValidArgumentLists();
    $a = $f->getValidArgumentLists();

    $this->assertEquals('$ver1, $ver2', $a[0]);
    $this->assertEquals('$ver1, $ver2, $oper', $a[1]);
  }

   public function testInitialisation() {

    $f = new gtFunction('version_compare');
    $f->setArgumentNames();
    $f->setInitialisationStatements();
    $a = $f->getInitialisationStatements();

    $this->assertEquals('$ver1 = ', $a[0]);
    $this->assertEquals('$ver2 = ', $a[1]);
    $this->assertEquals('$oper = ', $a[2]);
  }

}
?>
