<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id$
 */

  // {{{ class PHPTExpectancy
  //     Abstract base class for expectancies
  class PHPTExpectancy {
    var 
      $expected = '';
      
    function PHPTExpectancy($expected) {
      $this->expected= $expected;
    }

    function matches($output) { }
  }
  // }}}

  // {{{ class PHPTRegexExpectancy
  //     Expectancy class for regular expressions
  class PHPTRegexExpectancy extends PHPTExpectancy {
    
    function matches($output) { 
      return preg_match('°^'.strtr(preg_quote(rtrim($this->expected), '°'), array(
        '%s' => '(.+)',
        '%d' => '([0-9]+)'
      )).'°', $output);
    }
  }
  // }}}

  // {{{ class PHPTTest
  //     Represents a single .phpt-style test
  class PHPTTest {
    var
      $name         = '',
      $description  = '',
      $skipif       = '',
      $code         = '',
      $expectancy   = NULL,
      $output       = '';

    function &fromFile($filename) {
      $fd= fopen($filename, 'r');
      
      $sections= array();
      $current= NULL;
      while (!feof($fd)) {
        $line= fgets($fd, 0xFFFF);
        if (1 == sscanf($line, '--%[^-]--', $section)) {
          $sections[$section]= '';
          $current= $section;
          continue;
        }
        $sections[$current].= $line;
      }
      fclose($fd);
      
      // Create instance from read data and return it
      $t= &new PHPTTest(); {
        $t->name= substr(realpath($filename), 0, -1);
        $t->description= rtrim($sections['TEST']);
        $t->skipif= $sections['SKIPIF'];
        $t->code= $sections['FILE'];
        
        if (isset($sections['EXPECTF'])) {
          $t->expectancy= &new PHPTRegexExpectancy($sections['EXPECTF']);
        } else {
          // XXX TBI XXX
        }
      }      
      return $t;
    }
    
    function onError($errno, $errstr, $errfile, $errline) {
      static $names= array(
        E_NOTICE    => 'Notice',
        E_WARNING   => 'Warning'
      );
      
      if (!(error_reporting() & $errno)) return;
      printf(
        "\n%s: %s in %s on line %d\n", 
        $names[$errno], 
        $errstr,
        strstr($errfile, 'eval()\'d code') ? $this->name : $errfile,
        $errline
      );
    }
    
    function run() {

      // Precondition check - will die if test needs to be skipped
      eval('?>'.$this->skipif);
      
      set_error_handler(array(&$this, 'onError')); {
        error_reporting(E_ALL);

        ob_start();
        eval('?>'.$this->code);
        $this->output= rtrim(ob_get_contents());
        ob_end_clean();
      } restore_error_handler();
      
      return $this->expectancy->matches($this->output);
    }
  }
  // }}}
  
  // {{{ main
  if (isset($_GET['phpinfo'])) {
    phpinfo((int)$_GET['phpinfo']);

    echo '<a href="?">Home</a>';
    exit();
  }

  echo <<<__
  <html>
    <head>
      <title>PHPT Test</title>
      <style type="text/css">
        body {
          background-color: #ffffff; 
          color: #000000;
          font-size: 75%;
        }
        body, td, th, h1, h2 {
          font-family: sans-serif;
        }
        h1 {
          font-size: 150%;
        }
        h2 {
          font-size: 125%;
        }
        .header {
          background: url(?=PHPE9568F34-D428-11d2-A769-00AA001ACF42);
          background-position: right center;
          background-repeat: no-repeat;
          min-height: 70px;
          background-color: #9999cc;
          padding: 4px;
          padding-right: 120px;
          border: 1px solid #000000;
        }
        hr {
          width: 600px; 
          background-color: #cccccc; 
          border: 0px; 
          height: 1px; 
          color: #000000;
        }
      </style>
    </head>
    <body>
__;

  $test= basename($_SERVER['QUERY_STRING']);
  if ($test && file_exists($test)) {
    $t= &PHPTTest::fromFile($test);
    echo '<div class="header"><h1>'.basename($t->name), ': ', $t->description.'</h1></div>';
    echo '<a href="?">Back to test suite</a>';
    flush();

    // Run the test
    $result= $t->run();
    
    // Evaluate results
    if ($result) {
      echo '<h2>Passed</h2>';
    } else {
      echo '<h2>Failed</h2><hr/>';

      echo '<h3>Actual output</h3>';
      echo '<xmp>', $t->output, '</xmp><hr/>';

      echo '<h3>Expectancy</h3>';
      echo '<xmp>', $t->expectancy->expected, '</xmp>';
    }
    
    echo '<hr/>';
    exit();
  }
  
  echo '<div class="header"><h1>Test suite</h1></div>';
  
  // phpinfo() links
  echo 'phpinfo(): ';
  foreach (array(
    1   => 'General', 
    4   => 'Configuration', 
    8   => 'Modules'  
  ) as $const => $name) {
    printf('<a href="?phpinfo=%d">%s</a> | ', $const, $name);
  }
  echo '<a href="?phpinfo=-1">(All)</a>';
  
  echo '<h2>Select one to run</h2>';
  echo '<ul>';
  $d= dir(dirname(__FILE__));
  while ($entry= $d->read()) {
    if ('.phpt' != substr($entry, -5)) continue;
    echo '<li><a href="?'.$entry.'">'.$entry.'</a></li>';
  }
  $d->close();
  echo '</ul><hr/>';
  
  echo <<<__
    </body>
  </html>
__;
  // }}}
?>
