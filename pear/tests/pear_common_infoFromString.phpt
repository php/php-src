--TEST--
PEAR_Common::infoFromString test
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
if (!function_exists('token_get_all')) {
    echo 'skip';
}
?>
--FILE--
<?php
putenv('PHP_PEAR_SYSCONF_DIR=' . dirname(__FILE__));

require_once "PEAR/Common.php";

$common = &new PEAR_Common;

PEAR::setErrorHandling(PEAR_ERROR_CALLBACK, 'catchit');

function catchit($err)
{
    echo "Caught error: " . $err->getMessage() . "\n";
}

echo "Test invalid XML\n";

$common->infoFromString('\\goober');

echo "Test valid XML, not a package.xml\n";

$common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    "\n<grobbage></grobbage>");

echo "Test valid package.xml, invalid version number\n";

$common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="10000000"></package>');

echo "Test empty package.xml\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"></package>');

var_dump($ret);

echo "Test 1\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name></package>');

var_dump($ret);

echo "Test 2\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '</package>');

var_dump($ret);

echo "Test 3\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description></package>');

var_dump($ret);

echo "Test 4\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description><license>PHP License</license></package>');

var_dump($ret);

echo "Test 5\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description><license>PHP License</license>  <maintainers>
    <maintainer>
      <user>test</user>
      <role>lead</role>
      <name>test tester</name>
      <email>test@php.net</email>
    </maintainer></maintainers>
</package>');

var_dump($ret);

echo "Test 6\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description><license>PHP License</license>  <maintainers>
    <maintainer>
      <user>test</user>
      <role>lead</role>
      <name>test tester</name>
      <email>test@php.net</email>
    </maintainer></maintainers><release>
    <version>1.3b4</version></release>
</package>');

var_dump($ret);

echo "Test 7\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description><license>PHP License</license>  <maintainers>
    <maintainer>
      <user>test</user>
      <role>lead</role>
      <name>test tester</name>
      <email>test@php.net</email>
    </maintainer></maintainers><release>
    <version>1.3b4</version>
    <date>2003-11-17</date>
    <state>beta</state>
    <notes>test</notes></release>
</package>');

var_dump($ret);

echo "Test 8\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description><license>PHP License</license>  <maintainers>
    <maintainer>
      <user>test</user>
      <role>lead</role>
      <name>test tester</name>
      <email>test@php.net</email>
    </maintainer></maintainers><release>
    <version>1.3b4</version>
    <date>2003-11-17</date>
    <state>beta</state>
    <notes>test</notes>
    <provides type="class" name="furngy" /></release>
</package>');

var_dump($ret);

echo "Test 9\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description><license>PHP License</license>  <maintainers>
    <maintainer>
      <user>test</user>
      <role>lead</role>
      <name>test tester</name>
      <email>test@php.net</email>
    </maintainer></maintainers><release>
    <version>1.3b4</version>
    <date>2003-11-17</date>
    <state>beta</state>
    <notes>test</notes>
    <provides type="class" name="furngy" />
    <deps>
          <dep type="ext" rel="has" optional="yes">xmlrpc</dep>
    </deps>
</release>
</package>');

var_dump($ret);

echo "Test 10\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description><license>PHP License</license>  <maintainers>
    <maintainer>
      <user>test</user>
      <role>lead</role>
      <name>test tester</name>
      <email>test@php.net</email>
    </maintainer></maintainers><release>
    <version>1.3b4</version>
    <date>2003-11-17</date>
    <state>beta</state>
    <notes>test</notes>
    <provides type="class" name="furngy" />
    <deps>
          <dep type="ext" rel="has" optional="yes">xmlrpc</dep>
    </deps>
        <filelist>
      <file role="data" name="package.dtd"/>
      <file role="data" name="template.spec"/>
      <file role="php" name="PEAR.php"/>
      <file role="php" name="System.php"/>
      <dir name="PEAR">
        <file role="php" name="Autoloader.php"/>
        <file role="php" name="Command.php"/>
        <dir name="Command">
          <file role="php" name="Auth.php"/>
          <file role="php" name="Build.php"/>
          <file role="php" name="Common.php"/>
          <file role="php" name="Config.php"/>
          <file role="php" name="Install.php"/>
          <file role="php" name="Package.php"/>
          <file role="php" name="Registry.php"/>
          <file role="php" name="Remote.php"/>
          <file role="php" name="Mirror.php"/>
        </dir>
        <file role="php" name="Common.php"/>
        <file role="php" name="Config.php"/>
        <file role="php" name="Dependency.php"/>
        <dir name="Frontend">
          <file role="php" name="CLI.php"/>
        </dir>
        <file role="php" name="Builder.php"/>
        <file role="php" name="Installer.php"/>
        <file role="php" name="Packager.php"/>
        <file role="php" name="Registry.php"/>
        <file role="php" name="Remote.php"/>
      </dir>
      <dir name="OS">
        <file role="php" name="Guess.php"/>
      </dir>
      <dir name="scripts" baseinstalldir="/">
        <file role="script" install-as="pear" name="pear.sh">
          <replace from="@php_bin@" to="php_bin" type="pear-config"/>
          <replace from="@php_dir@" to="php_dir" type="pear-config"/>
          <replace from="@pear_version@" to="version" type="package-info"/>
          <replace from="@include_path@" to="php_dir" type="pear-config"/>
        </file>
        <file role="script" platform="windows" install-as="pear.bat" name="pear.bat">
        <replace from="@bin_dir@" to="bin_dir" type="pear-config"/>
        <replace from="@php_bin@" to="php_bin" type="pear-config"/>
        <replace from="@include_path@" to="php_dir" type="pear-config"/>
        </file>
        <file role="php" install-as="pearcmd.php" name="pearcmd.php">
          <replace from="@php_bin@" to="php_bin" type="pear-config"/>
          <replace from="@php_dir@" to="php_dir" type="pear-config"/>
          <replace from="@pear_version@" to="version" type="package-info"/>
          <replace from="@include_path@" to="php_dir" type="pear-config"/>
        </file>
      </dir>
    </filelist>

</release>
</package>');

var_dump($ret);

echo "Test 11\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description><license>PHP License</license>  <maintainers>
    <maintainer>
      <user>test</user>
      <role>lead</role>
      <name>test tester</name>
      <email>test@php.net</email>
    </maintainer></maintainers><release>
    <version>1.3b4</version>
    <date>2003-11-17</date>
    <state>beta</state>
    <notes>test</notes>
    <provides type="class" name="furngy" />
    <deps>
          <dep type="ext" rel="has" optional="yes">xmlrpc</dep>
    </deps>
        <filelist>
      <file role="data" name="package.dtd"/>
      <file role="data" name="template.spec"/>
      <file role="php" name="PEAR.php"/>
      <file role="php" name="System.php"/>
      <dir name="PEAR">
        <file role="php" name="Autoloader.php"/>
        <file role="php" name="Command.php"/>
        <dir name="Command">
          <file role="php" name="Auth.php"/>
          <file role="php" name="Build.php"/>
          <file role="php" name="Common.php"/>
          <file role="php" name="Config.php"/>
          <file role="php" name="Install.php"/>
          <file role="php" name="Package.php"/>
          <file role="php" name="Registry.php"/>
          <file role="php" name="Remote.php"/>
          <file role="php" name="Mirror.php"/>
        </dir>
        <file role="php" name="Common.php"/>
        <file role="php" name="Config.php"/>
        <file role="php" name="Dependency.php"/>
        <dir name="Frontend">
          <file role="php" name="CLI.php"/>
        </dir>
        <file role="php" name="Builder.php"/>
        <file role="php" name="Installer.php"/>
        <file role="php" name="Packager.php"/>
        <file role="php" name="Registry.php"/>
        <file role="php" name="Remote.php"/>
      </dir>
      <dir name="OS">
        <file role="php" name="Guess.php"/>
      </dir>
      <dir name="scripts" baseinstalldir="/">
        <file role="script" install-as="pear" name="pear.sh">
          <replace from="@php_bin@" to="php_bin" type="pear-config"/>
          <replace from="@php_dir@" to="php_dir" type="pear-config"/>
          <replace from="@pear_version@" to="version" type="package-info"/>
          <replace from="@include_path@" to="php_dir" type="pear-config"/>
        </file>
        <file role="script" platform="windows" install-as="pear.bat" name="pear.bat">
        <replace from="@bin_dir@" to="bin_dir" type="pear-config"/>
        <replace from="@php_bin@" to="php_bin" type="pear-config"/>
        <replace from="@include_path@" to="php_dir" type="pear-config"/>
        </file>
        <file role="php" install-as="pearcmd.php" name="pearcmd.php">
          <replace from="@php_bin@" to="php_bin" type="pear-config"/>
          <replace from="@php_dir@" to="php_dir" type="pear-config"/>
          <replace from="@pear_version@" to="version" type="package-info"/>
          <replace from="@include_path@" to="php_dir" type="pear-config"/>
        </file>
      </dir>
    </filelist>
    <configureoptions>
     <configureoption name="test" prompt="The prompt test" default="foo" />
    </configureoptions>
</release>
</package>');

var_dump($ret);

echo "Test 12\n";

$ret = $common->infoFromString('<?xml version="1.0" encoding="ISO-8859-1" ?>' .
    '<package version="1.0"><name>test</name><summary>PEAR test</summary>' . 
    '<description>The test</description><license>PHP License</license>  <maintainers>
    <maintainer>
      <user>test</user>
      <role>lead</role>
      <name>test tester</name>
      <email>test@php.net</email>
    </maintainer></maintainers><release>
    <version>1.3b4</version>
    <date>2003-11-17</date>
    <state>beta</state>
    <notes>test</notes>
    <provides type="class" name="furngy" />
    <deps>
          <dep type="ext" rel="has" optional="yes">xmlrpc</dep>
    </deps>
        <filelist>
      <file role="data" name="package.dtd"/>
      <file role="data" name="template.spec"/>
      <file role="php" name="PEAR.php"/>
      <file role="php" name="System.php"/>
      <dir name="PEAR">
        <file role="php" name="Autoloader.php"/>
        <file role="php" name="Command.php"/>
        <dir name="Command">
          <file role="php" name="Auth.php"/>
          <file role="php" name="Build.php"/>
          <file role="php" name="Common.php"/>
          <file role="php" name="Config.php"/>
          <file role="php" name="Install.php"/>
          <file role="php" name="Package.php"/>
          <file role="php" name="Registry.php"/>
          <file role="php" name="Remote.php"/>
          <file role="php" name="Mirror.php"/>
        </dir>
        <file role="php" name="Common.php"/>
        <file role="php" name="Config.php"/>
        <file role="php" name="Dependency.php"/>
        <dir name="Frontend">
          <file role="php" name="CLI.php"/>
        </dir>
        <file role="php" name="Builder.php"/>
        <file role="php" name="Installer.php"/>
        <file role="php" name="Packager.php"/>
        <file role="php" name="Registry.php"/>
        <file role="php" name="Remote.php"/>
      </dir>
      <dir name="OS">
        <file role="php" name="Guess.php"/>
      </dir>
      <dir name="scripts" baseinstalldir="/">
        <file role="script" install-as="pear" name="pear.sh">
          <replace from="@php_bin@" to="php_bin" type="pear-config"/>
          <replace from="@php_dir@" to="php_dir" type="pear-config"/>
          <replace from="@pear_version@" to="version" type="package-info"/>
          <replace from="@include_path@" to="php_dir" type="pear-config"/>
        </file>
        <file role="script" platform="windows" install-as="pear.bat" name="pear.bat">
        <replace from="@bin_dir@" to="bin_dir" type="pear-config"/>
        <replace from="@php_bin@" to="php_bin" type="pear-config"/>
        <replace from="@include_path@" to="php_dir" type="pear-config"/>
        </file>
        <file role="php" install-as="pearcmd.php" name="pearcmd.php">
          <replace from="@php_bin@" to="php_bin" type="pear-config"/>
          <replace from="@php_dir@" to="php_dir" type="pear-config"/>
          <replace from="@pear_version@" to="version" type="package-info"/>
          <replace from="@include_path@" to="php_dir" type="pear-config"/>
        </file>
      </dir>
    </filelist>
    <configureoptions>
     <configureoption name="test" prompt="The prompt test" default="foo" />
    </configureoptions>
</release>
  <changelog>
    <release>
      <version>0.1</version>
      <date>2003-07-21</date>
      <license>PHP License</license>
      <state>alpha</state>
      <notes>First release of test</notes>
    </release>
    <release>
      <version>0.2</version>
      <date>2003-07-21</date>
      <license>PHP License</license>
      <state>alpha</state>
      <notes>Generation of package.xml from scratch is now supported.  In addition,
generation of &lt;provides&gt; is supported and so is addition of
maintainers and configure options

- Fixed a bug in &lt;release&gt; generation
- Added _addProvides() to generate a &lt;provides&gt; section</notes>
    </release>
   </changelog>
</package>');

var_dump($ret);

?>
--GET--
--POST--
--EXPECT--
Test invalid XML
Caught error: XML error: not well-formed (invalid token) at line 1
Test valid XML, not a package.xml
Caught error: Invalid Package File, no <package> tag
Test valid package.xml, invalid version number
Caught error: No handlers for package.xml version 10000000
Test empty package.xml
array(2) {
  ["provides"]=>
  array(0) {
  }
  ["filelist"]=>
  &array(0) {
  }
}
Test 1
array(3) {
  ["provides"]=>
  array(0) {
  }
  ["filelist"]=>
  &array(0) {
  }
  ["package"]=>
  string(4) "test"
}
Test 2
array(4) {
  ["provides"]=>
  array(0) {
  }
  ["filelist"]=>
  &array(0) {
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
}
Test 3
array(5) {
  ["provides"]=>
  array(0) {
  }
  ["filelist"]=>
  &array(0) {
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
}
Test 4
array(6) {
  ["provides"]=>
  array(0) {
  }
  ["filelist"]=>
  &array(0) {
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
  ["release_license"]=>
  string(11) "PHP License"
}
Test 5
array(7) {
  ["provides"]=>
  array(0) {
  }
  ["filelist"]=>
  &array(0) {
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
  ["release_license"]=>
  string(11) "PHP License"
  ["maintainers"]=>
  array(1) {
    [0]=>
    &array(4) {
      ["handle"]=>
      string(4) "test"
      ["role"]=>
      string(4) "lead"
      ["name"]=>
      string(11) "test tester"
      ["email"]=>
      string(12) "test@php.net"
    }
  }
}
Test 6
array(8) {
  ["provides"]=>
  array(0) {
  }
  ["filelist"]=>
  &array(0) {
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
  ["release_license"]=>
  string(11) "PHP License"
  ["maintainers"]=>
  array(1) {
    [0]=>
    &array(4) {
      ["handle"]=>
      string(4) "test"
      ["role"]=>
      string(4) "lead"
      ["name"]=>
      string(11) "test tester"
      ["email"]=>
      string(12) "test@php.net"
    }
  }
  ["version"]=>
  string(5) "1.3b4"
}
Test 7
array(11) {
  ["provides"]=>
  array(0) {
  }
  ["filelist"]=>
  &array(0) {
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
  ["release_license"]=>
  string(11) "PHP License"
  ["maintainers"]=>
  array(1) {
    [0]=>
    &array(4) {
      ["handle"]=>
      string(4) "test"
      ["role"]=>
      string(4) "lead"
      ["name"]=>
      string(11) "test tester"
      ["email"]=>
      string(12) "test@php.net"
    }
  }
  ["version"]=>
  string(5) "1.3b4"
  ["release_date"]=>
  string(10) "2003-11-17"
  ["release_state"]=>
  string(4) "beta"
  ["release_notes"]=>
  string(4) "test"
}
Test 8
array(11) {
  ["provides"]=>
  array(1) {
    ["class;furngy"]=>
    array(3) {
      ["type"]=>
      string(5) "class"
      ["name"]=>
      string(6) "furngy"
      ["explicit"]=>
      bool(true)
    }
  }
  ["filelist"]=>
  &array(0) {
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
  ["release_license"]=>
  string(11) "PHP License"
  ["maintainers"]=>
  array(1) {
    [0]=>
    &array(4) {
      ["handle"]=>
      string(4) "test"
      ["role"]=>
      string(4) "lead"
      ["name"]=>
      string(11) "test tester"
      ["email"]=>
      string(12) "test@php.net"
    }
  }
  ["version"]=>
  string(5) "1.3b4"
  ["release_date"]=>
  string(10) "2003-11-17"
  ["release_state"]=>
  string(4) "beta"
  ["release_notes"]=>
  string(4) "test"
}
Test 9
array(12) {
  ["provides"]=>
  array(1) {
    ["class;furngy"]=>
    array(3) {
      ["type"]=>
      string(5) "class"
      ["name"]=>
      string(6) "furngy"
      ["explicit"]=>
      bool(true)
    }
  }
  ["filelist"]=>
  &array(0) {
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
  ["release_license"]=>
  string(11) "PHP License"
  ["maintainers"]=>
  array(1) {
    [0]=>
    &array(4) {
      ["handle"]=>
      string(4) "test"
      ["role"]=>
      string(4) "lead"
      ["name"]=>
      string(11) "test tester"
      ["email"]=>
      string(12) "test@php.net"
    }
  }
  ["version"]=>
  string(5) "1.3b4"
  ["release_date"]=>
  string(10) "2003-11-17"
  ["release_state"]=>
  string(4) "beta"
  ["release_notes"]=>
  string(4) "test"
  ["release_deps"]=>
  array(1) {
    [1]=>
    array(4) {
      ["type"]=>
      string(3) "ext"
      ["rel"]=>
      string(3) "has"
      ["optional"]=>
      string(3) "yes"
      ["name"]=>
      string(6) "xmlrpc"
    }
  }
}
Test 10
array(12) {
  ["provides"]=>
  array(1) {
    ["class;furngy"]=>
    array(3) {
      ["type"]=>
      string(5) "class"
      ["name"]=>
      string(6) "furngy"
      ["explicit"]=>
      bool(true)
    }
  }
  ["filelist"]=>
  &array(28) {
    ["package.dtd"]=>
    array(1) {
      ["role"]=>
      string(4) "data"
    }
    ["template.spec"]=>
    array(1) {
      ["role"]=>
      string(4) "data"
    }
    ["PEAR.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["System.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Autoloader.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Auth.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Build.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Common.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Config.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Install.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Package.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Registry.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Remote.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Mirror.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Common.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Config.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Dependency.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Frontend\CLI.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Builder.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Installer.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Packager.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Registry.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Remote.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["OS\Guess.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["scripts\pear.sh"]=>
    array(4) {
      ["role"]=>
      string(6) "script"
      ["install-as"]=>
      string(4) "pear"
      ["baseinstalldir"]=>
      string(1) "/"
      ["replacements"]=>
      array(4) {
        [0]=>
        array(3) {
          ["from"]=>
          string(9) "@php_bin@"
          ["to"]=>
          string(7) "php_bin"
          ["type"]=>
          string(11) "pear-config"
        }
        [1]=>
        array(3) {
          ["from"]=>
          string(9) "@php_dir@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
        [2]=>
        array(3) {
          ["from"]=>
          string(14) "@pear_version@"
          ["to"]=>
          string(7) "version"
          ["type"]=>
          string(12) "package-info"
        }
        [3]=>
        array(3) {
          ["from"]=>
          string(14) "@include_path@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
      }
    }
    ["scripts\pear.bat"]=>
    array(5) {
      ["role"]=>
      string(6) "script"
      ["platform"]=>
      string(7) "windows"
      ["install-as"]=>
      string(8) "pear.bat"
      ["baseinstalldir"]=>
      string(1) "/"
      ["replacements"]=>
      array(3) {
        [0]=>
        array(3) {
          ["from"]=>
          string(9) "@bin_dir@"
          ["to"]=>
          string(7) "bin_dir"
          ["type"]=>
          string(11) "pear-config"
        }
        [1]=>
        array(3) {
          ["from"]=>
          string(9) "@php_bin@"
          ["to"]=>
          string(7) "php_bin"
          ["type"]=>
          string(11) "pear-config"
        }
        [2]=>
        array(3) {
          ["from"]=>
          string(14) "@include_path@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
      }
    }
    ["scripts\pearcmd.php"]=>
    array(4) {
      ["role"]=>
      string(3) "php"
      ["install-as"]=>
      string(11) "pearcmd.php"
      ["baseinstalldir"]=>
      string(1) "/"
      ["replacements"]=>
      array(4) {
        [0]=>
        array(3) {
          ["from"]=>
          string(9) "@php_bin@"
          ["to"]=>
          string(7) "php_bin"
          ["type"]=>
          string(11) "pear-config"
        }
        [1]=>
        array(3) {
          ["from"]=>
          string(9) "@php_dir@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
        [2]=>
        array(3) {
          ["from"]=>
          string(14) "@pear_version@"
          ["to"]=>
          string(7) "version"
          ["type"]=>
          string(12) "package-info"
        }
        [3]=>
        array(3) {
          ["from"]=>
          string(14) "@include_path@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
      }
    }
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
  ["release_license"]=>
  string(11) "PHP License"
  ["maintainers"]=>
  array(1) {
    [0]=>
    &array(4) {
      ["handle"]=>
      string(4) "test"
      ["role"]=>
      string(4) "lead"
      ["name"]=>
      string(11) "test tester"
      ["email"]=>
      string(12) "test@php.net"
    }
  }
  ["version"]=>
  string(5) "1.3b4"
  ["release_date"]=>
  string(10) "2003-11-17"
  ["release_state"]=>
  string(4) "beta"
  ["release_notes"]=>
  string(4) "test"
  ["release_deps"]=>
  array(1) {
    [1]=>
    array(4) {
      ["type"]=>
      string(3) "ext"
      ["rel"]=>
      string(3) "has"
      ["optional"]=>
      string(3) "yes"
      ["name"]=>
      string(6) "xmlrpc"
    }
  }
}
Test 11
array(13) {
  ["provides"]=>
  array(1) {
    ["class;furngy"]=>
    array(3) {
      ["type"]=>
      string(5) "class"
      ["name"]=>
      string(6) "furngy"
      ["explicit"]=>
      bool(true)
    }
  }
  ["filelist"]=>
  &array(28) {
    ["package.dtd"]=>
    array(1) {
      ["role"]=>
      string(4) "data"
    }
    ["template.spec"]=>
    array(1) {
      ["role"]=>
      string(4) "data"
    }
    ["PEAR.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["System.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Autoloader.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Auth.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Build.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Common.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Config.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Install.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Package.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Registry.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Remote.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Mirror.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Common.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Config.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Dependency.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Frontend\CLI.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Builder.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Installer.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Packager.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Registry.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Remote.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["OS\Guess.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["scripts\pear.sh"]=>
    array(4) {
      ["role"]=>
      string(6) "script"
      ["install-as"]=>
      string(4) "pear"
      ["baseinstalldir"]=>
      string(1) "/"
      ["replacements"]=>
      array(4) {
        [0]=>
        array(3) {
          ["from"]=>
          string(9) "@php_bin@"
          ["to"]=>
          string(7) "php_bin"
          ["type"]=>
          string(11) "pear-config"
        }
        [1]=>
        array(3) {
          ["from"]=>
          string(9) "@php_dir@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
        [2]=>
        array(3) {
          ["from"]=>
          string(14) "@pear_version@"
          ["to"]=>
          string(7) "version"
          ["type"]=>
          string(12) "package-info"
        }
        [3]=>
        array(3) {
          ["from"]=>
          string(14) "@include_path@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
      }
    }
    ["scripts\pear.bat"]=>
    array(5) {
      ["role"]=>
      string(6) "script"
      ["platform"]=>
      string(7) "windows"
      ["install-as"]=>
      string(8) "pear.bat"
      ["baseinstalldir"]=>
      string(1) "/"
      ["replacements"]=>
      array(3) {
        [0]=>
        array(3) {
          ["from"]=>
          string(9) "@bin_dir@"
          ["to"]=>
          string(7) "bin_dir"
          ["type"]=>
          string(11) "pear-config"
        }
        [1]=>
        array(3) {
          ["from"]=>
          string(9) "@php_bin@"
          ["to"]=>
          string(7) "php_bin"
          ["type"]=>
          string(11) "pear-config"
        }
        [2]=>
        array(3) {
          ["from"]=>
          string(14) "@include_path@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
      }
    }
    ["scripts\pearcmd.php"]=>
    array(4) {
      ["role"]=>
      string(3) "php"
      ["install-as"]=>
      string(11) "pearcmd.php"
      ["baseinstalldir"]=>
      string(1) "/"
      ["replacements"]=>
      array(4) {
        [0]=>
        array(3) {
          ["from"]=>
          string(9) "@php_bin@"
          ["to"]=>
          string(7) "php_bin"
          ["type"]=>
          string(11) "pear-config"
        }
        [1]=>
        array(3) {
          ["from"]=>
          string(9) "@php_dir@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
        [2]=>
        array(3) {
          ["from"]=>
          string(14) "@pear_version@"
          ["to"]=>
          string(7) "version"
          ["type"]=>
          string(12) "package-info"
        }
        [3]=>
        array(3) {
          ["from"]=>
          string(14) "@include_path@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
      }
    }
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
  ["release_license"]=>
  string(11) "PHP License"
  ["maintainers"]=>
  array(1) {
    [0]=>
    &array(4) {
      ["handle"]=>
      string(4) "test"
      ["role"]=>
      string(4) "lead"
      ["name"]=>
      string(11) "test tester"
      ["email"]=>
      string(12) "test@php.net"
    }
  }
  ["version"]=>
  string(5) "1.3b4"
  ["release_date"]=>
  string(10) "2003-11-17"
  ["release_state"]=>
  string(4) "beta"
  ["release_notes"]=>
  string(4) "test"
  ["release_deps"]=>
  array(1) {
    [1]=>
    array(4) {
      ["type"]=>
      string(3) "ext"
      ["rel"]=>
      string(3) "has"
      ["optional"]=>
      string(3) "yes"
      ["name"]=>
      string(6) "xmlrpc"
    }
  }
  ["configure_options"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(4) "test"
      ["prompt"]=>
      string(15) "The prompt test"
      ["default"]=>
      string(3) "foo"
    }
  }
}
Test 12
array(14) {
  ["provides"]=>
  array(1) {
    ["class;furngy"]=>
    array(3) {
      ["type"]=>
      string(5) "class"
      ["name"]=>
      string(6) "furngy"
      ["explicit"]=>
      bool(true)
    }
  }
  ["filelist"]=>
  &array(28) {
    ["package.dtd"]=>
    array(1) {
      ["role"]=>
      string(4) "data"
    }
    ["template.spec"]=>
    array(1) {
      ["role"]=>
      string(4) "data"
    }
    ["PEAR.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["System.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Autoloader.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Auth.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Build.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Common.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Config.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Install.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Package.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Registry.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Remote.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Command\Mirror.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Common.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Config.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Dependency.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Frontend\CLI.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Builder.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Installer.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Packager.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Registry.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["PEAR\Remote.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["OS\Guess.php"]=>
    array(1) {
      ["role"]=>
      string(3) "php"
    }
    ["scripts\pear.sh"]=>
    array(4) {
      ["role"]=>
      string(6) "script"
      ["install-as"]=>
      string(4) "pear"
      ["baseinstalldir"]=>
      string(1) "/"
      ["replacements"]=>
      array(4) {
        [0]=>
        array(3) {
          ["from"]=>
          string(9) "@php_bin@"
          ["to"]=>
          string(7) "php_bin"
          ["type"]=>
          string(11) "pear-config"
        }
        [1]=>
        array(3) {
          ["from"]=>
          string(9) "@php_dir@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
        [2]=>
        array(3) {
          ["from"]=>
          string(14) "@pear_version@"
          ["to"]=>
          string(7) "version"
          ["type"]=>
          string(12) "package-info"
        }
        [3]=>
        array(3) {
          ["from"]=>
          string(14) "@include_path@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
      }
    }
    ["scripts\pear.bat"]=>
    array(5) {
      ["role"]=>
      string(6) "script"
      ["platform"]=>
      string(7) "windows"
      ["install-as"]=>
      string(8) "pear.bat"
      ["baseinstalldir"]=>
      string(1) "/"
      ["replacements"]=>
      array(3) {
        [0]=>
        array(3) {
          ["from"]=>
          string(9) "@bin_dir@"
          ["to"]=>
          string(7) "bin_dir"
          ["type"]=>
          string(11) "pear-config"
        }
        [1]=>
        array(3) {
          ["from"]=>
          string(9) "@php_bin@"
          ["to"]=>
          string(7) "php_bin"
          ["type"]=>
          string(11) "pear-config"
        }
        [2]=>
        array(3) {
          ["from"]=>
          string(14) "@include_path@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
      }
    }
    ["scripts\pearcmd.php"]=>
    array(4) {
      ["role"]=>
      string(3) "php"
      ["install-as"]=>
      string(11) "pearcmd.php"
      ["baseinstalldir"]=>
      string(1) "/"
      ["replacements"]=>
      array(4) {
        [0]=>
        array(3) {
          ["from"]=>
          string(9) "@php_bin@"
          ["to"]=>
          string(7) "php_bin"
          ["type"]=>
          string(11) "pear-config"
        }
        [1]=>
        array(3) {
          ["from"]=>
          string(9) "@php_dir@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
        [2]=>
        array(3) {
          ["from"]=>
          string(14) "@pear_version@"
          ["to"]=>
          string(7) "version"
          ["type"]=>
          string(12) "package-info"
        }
        [3]=>
        array(3) {
          ["from"]=>
          string(14) "@include_path@"
          ["to"]=>
          string(7) "php_dir"
          ["type"]=>
          string(11) "pear-config"
        }
      }
    }
  }
  ["package"]=>
  string(4) "test"
  ["summary"]=>
  string(9) "PEAR test"
  ["description"]=>
  string(8) "The test"
  ["release_license"]=>
  string(11) "PHP License"
  ["maintainers"]=>
  array(1) {
    [0]=>
    &array(4) {
      ["handle"]=>
      string(4) "test"
      ["role"]=>
      string(4) "lead"
      ["name"]=>
      string(11) "test tester"
      ["email"]=>
      string(12) "test@php.net"
    }
  }
  ["version"]=>
  string(5) "1.3b4"
  ["release_date"]=>
  string(10) "2003-11-17"
  ["release_state"]=>
  string(4) "beta"
  ["release_notes"]=>
  string(4) "test"
  ["release_deps"]=>
  array(1) {
    [1]=>
    array(4) {
      ["type"]=>
      string(3) "ext"
      ["rel"]=>
      string(3) "has"
      ["optional"]=>
      string(3) "yes"
      ["name"]=>
      string(6) "xmlrpc"
    }
  }
  ["configure_options"]=>
  array(1) {
    [0]=>
    array(3) {
      ["name"]=>
      string(4) "test"
      ["prompt"]=>
      string(15) "The prompt test"
      ["default"]=>
      string(3) "foo"
    }
  }
  ["changelog"]=>
  array(2) {
    [0]=>
    &array(5) {
      ["version"]=>
      string(3) "0.1"
      ["release_date"]=>
      string(10) "2003-07-21"
      ["release_license"]=>
      string(11) "PHP License"
      ["release_state"]=>
      string(5) "alpha"
      ["release_notes"]=>
      string(22) "First release of test
"
    }
    [1]=>
    &array(5) {
      ["version"]=>
      string(3) "0.2"
      ["release_date"]=>
      string(10) "2003-07-21"
      ["release_license"]=>
      string(11) "PHP License"
      ["release_state"]=>
      string(5) "alpha"
      ["release_notes"]=>
      string(260) "Generation of package.xml from scratch is now supported.  In addition,
generation of <provides> is supported and so is addition of
maintainers and configure options

- Fixed a bug in <release> generation
- Added _addProvides() to generate a <provides> section
"
    }
  }
}