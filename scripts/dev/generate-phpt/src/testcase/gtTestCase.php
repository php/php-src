<?php

/**
 * Class for all test cases
 */
abstract class gtTestCase {


  /**
   * The subject of the test, may be either a function (gtFunction) or a method (gtMethod)
   *
   * @var gtMethod or gtFunction
   */
  protected $subject;


  /**
   * Arry of strings containing the test case
   *
   * @var array
   */
  protected $testCase;


  /**
   * Object containing the optional sections that may be added to the test case
   *
   * @var gtOptionalSections
   */
  protected $optionalSections;


  /**
   * Convert test case from array to string
   *
   * @return string
   */
  public function toString() {
    $testCaseString = "";
    foreach($this->testCase as $line) {
      $testCaseString .= $line."\n";
    }
    return $testCaseString;
  }



  /**
   * Returns test case as a array
   *
   * @return array
   */
  public function getTestCase() {
    return $this->testCase;
  }


  /**
   * Construct the common headers (title, file section..) of the test case
   *
   */
  public function ConstructCommonHeaders() {
    $this->testHeader();

    if($this->optionalSections->hasSkipif()) {
      $this->addSkipif();
    }

    if($this->optionalSections->hasIni()) {
      $this->addIni();
    }

    $this->fileOpening();
  }


  /**
   * Construct the common closing statements (clean, done, EXPECTF...)
   *
   */
  public function ConstructCommonClosing() {
    $this->fileClosing();
     
    if ($this->optionalSections->hasDone()) {
      $this->addDone();
    }
     
    if ($this->optionalSections->hasClean()) {
      $this->addClean();
    }

    $this->addExpectf();
  }

  /**
   * Start the FILE section of the test
   *
   */
  public function fileOpening() {
    $this->testCase[] = "--FILE--";
    $this->testCase[] = "<?php";
    $this->testCase = gtCodeSnippet::appendBlankLines(2, $this->testCase );
  }


  /**
   * Add contructor argument initialisation to test case
   *
   */
  public function constructorArgInit() {
    $conStatements = $this->subject->getConstructorInitStatements();
    foreach($conStatements as $statement) {
      $this->testCase[] = $statement;
    }
  }


  /**
   * Create instance of class in the test case
   *
   */
  public function constructorCreateInstance() {
    $constructorList = $this->subject->getConstructorArgumentList();
    $this->testCase[] = "\$class = new ".$this->subject->getClassName()."( ".$constructorList." );";
    $this->testCase = gtCodeSnippet::appendBlankLines(2, $this->testCase );
  }


  /**
   * Add function or method initilaisation statements to the test case
   *
   */
  public function argInit() {
    $statements = $this->subject->getInitialisationStatements();
    foreach($statements as $statement) {
      $this->testCase[] = $statement;
    }
    $this->testCase = gtCodeSnippet::appendBlankLines(2, $this->testCase );
  }


  /**
   * Add FILE section closing tag to the test case
   *
   */
  public function fileClosing() {
    $this->testCase[] = "?>";
  }


  /**
   * Add a skipif section to the test case
   *
   */
  public function addSkipif() {
    $this->testCase[] = "--SKIPIF--";
    $this->testCase[] = "<?php";
    if($this->optionalSections->hasSkipifKey()) {
      $key = $this->optionalSections->getSkipifKey();
      //test standard skipif sections
      if($key == 'win') {
        $this->testCase = gtCodeSnippet::append('skipifwin', $this->testCase);
      }
      if($key == 'notwin' ) {
        $this->testCase = gtCodeSnippet::append('skipifnotwin', $this->testCase);
      }

      if($key == '64b' ) {
        $this->testCase = gtCodeSnippet::append('skipif64b', $this->testCase);
      }

      if($key == 'not64b' ) {
        $this->testCase = gtCodeSnippet::append('skipifnot64b', $this->testCase);
      }
    }

    if($this->optionalSections->hasSkipifExt()) {
      $ext = $this->optionalSections->getSkipifExt();
      $this->testCase[] = "if (!extension_loaded('$ext')) die ('skip $ext extension not available in this build');";
    }
    $this->testCase[] = "?>";
  }


  /**
   * Add an INI section to the test case
   *
   */
  public function addIni() {
    $this->testCase[] = "--INI--";
    $this->testCase[] = "";
  }


  /**
   * Add a clean section to the test case
   *
   */
  public function addClean() {
    $this->testCase[] = "--CLEAN--";
    $this->testCase[] = "<?php";
    $this->testCase[] = "?>";
  }


  /**
   * Add a ===DONE=== statement to the test case
   *
   */
  public function addDone() {
    $this->testCase[] = "===DONE===";
  }


  /**
   * Add an EXPECTF section
   *
   */
  public function addExpectf() {
    $this->testCase[] = "--EXPECTF--";
    if ($this->optionalSections->hasDone() ){
      $this->testCase[] = '===DONE===';
    }
  }

  public function getOpt() {
    return $this->optionalSections;
  }
}
?>
