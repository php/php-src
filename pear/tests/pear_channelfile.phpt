--TEST--
PEAR_Channelfile
--SKIPIF--
<?php
if (!getenv('PHP_PEAR_RUNTESTS')) {
    echo 'skip';
}
?>
--FILE--
<?php

error_reporting(E_ALL);
chdir(dirname(__FILE__));
include "PEAR/Channelfile.php";
require_once 'PEAR/ErrorStack.php';

function logStack($err)
{
    echo "caught Error Stack error:\n";
    echo $err['message'] . "\n";
    echo 'code : ' . $err['code'] . "\n";
}
PEAR_ErrorStack::setDefaultCallback('logStack');
echo "test default\n";
$chf = new PEAR_ChannelFile;
$chf->fromXmlString('<?xml version="1.0" encoding="ISO-8859-1" ?>
<channel version="1.0">
 <name>PEAR5</name>
<summary>PEAR packages for PHP 5</summary>
<server>pear.php.net</server>
<subchannels>
<subchannel name="PEAR5.qa">
<server>qa.pear.php.net</server>
<summary>Quality Assurance releases of unmaintained packages</summary>
<deps><dep type="pkg" rel="ge" version="1.4">PEAR</dep>
<dep type="php" rel="ge" version="5.0.0"/></deps>
</subchannel>
<subchannel name="pear.boo">
<server>boo.pear.php.net</server>
<summary>booya</summary>
<deps><dep type="pkg" rel="ge" version="1.5">PEAR</dep>
<dep type="php" rel="ge" version="5.1.0"/></deps>
</subchannel>
</subchannels>
<deps>
 <dep type="php" rel="ge" version="5.0.0"/>
</deps>
</channel>');

echo "after parsing\n";
var_dump($chf->validate());
var_dump($chf->toArray());
var_dump($chf->toXml());
$chf->fromXmlString($chf->toXml());

echo "after re-parsing\n";
var_dump($chf->validate());
var_dump($chf->toArray());
var_dump($chf->toXml());

echo "test compatibility\n";
$chf = new PEAR_ChannelFile(true);
$chf->fromXmlString('<?xml version="1.0" encoding="ISO-8859-1" ?>
<channel version="1.0">
 <name>PEAR5</name>
<summary>PEAR packages for PHP 5</summary>
<server>pear.php.net</server>
<subchannels>
<subchannel name="PEAR5.qa">
<server>qa.pear.php.net</server>
<summary>Quality Assurance releases of unmaintained packages</summary>
<deps><dep type="pkg" rel="ge" version="1.4">PEAR</dep>
<dep type="php" rel="ge" version="5.0.0"/></deps>
</subchannel>
<subchannel name="pear.boo">
<server>boo.pear.php.net</server>
<summary>booya</summary>
<deps><dep type="pkg" rel="ge" version="1.5">PEAR</dep>
<dep type="php" rel="ge" version="5.1.0"/></deps>
</subchannel>
</subchannels>
<deps>
 <dep type="php" rel="ge" version="5.0.0"/>
</deps>
</channel>');

echo "after parsing\n";
var_dump($chf->validate());
var_dump($chf->toArray());
var_dump($chf->toXml());
$chf->fromXmlString($chf->toXml());

echo "after re-parsing\n";
var_dump($chf->validate());
var_dump($chf->toArray());
var_dump($chf->toXml());

echo "\ntest add packagenameregex\n";
$chf->setPackageNameRegex('[A-Z]+');
var_dump($chf->toArray());
var_dump($chf->toXml());
$chf->fromXmlString($chf->toXml());

echo "after re-parsing\n";
var_dump($chf->validate());
var_dump($chf->toArray());
var_dump($chf->toXml());

echo "\ntest add protocols\n";
$chf->addProtocol('xml-rpc', '1.0', 'package.listall');
$chf->addProtocol('xml-rpc', '1.0', 'release.list');
$chf->addProtocol('get', '1.0');
var_dump($chf->toArray());
var_dump($chf->toXml());
$chf->fromXmlString($chf->toXml());

echo "after re-parsing\n";
var_dump($chf->validate());
var_dump($chf->toArray());
var_dump($chf->toXml());

echo "\ntest add mirror\n";
$chf->addMirror('server', 'mirror.php.net');
$chf->addMirrorProtocol('mirror.php.net', 'xml-rpc', '1.0', 'package.listall');
$chf->addMirrorProtocol('mirror.php.net', 'xml-rpc', '1.0', 'release.list');
var_dump($chf->toArray());
var_dump($chf->toXml());
$chf->fromXmlString($chf->toXml());

echo "after re-parsing\n";
var_dump($chf->validate());
var_dump($chf->toArray());
var_dump($chf->toXml());
?>
--EXPECT--
test default
after parsing
bool(true)
array(6) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
}
string(823) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"
after re-parsing
bool(true)
array(6) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
}
string(823) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"
test compatibility
after parsing
bool(true)
array(6) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
}
string(823) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"
after re-parsing
bool(true)
array(6) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
}
string(823) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"

test add packagenameregex
array(7) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
  ["packagenameregex"]=>
  string(6) "[A-Z]+"
}
string(868) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <packagenameregex>[A-Z]+</packagenameregex>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"
after re-parsing
bool(true)
array(7) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["packagenameregex"]=>
  string(6) "[A-Z]+"
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
}
string(868) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <packagenameregex>[A-Z]+</packagenameregex>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"

test add protocols
array(8) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["packagenameregex"]=>
  string(6) "[A-Z]+"
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
  ["provides"]=>
  array(3) {
    [1]=>
    array(3) {
      ["type"]=>
      string(7) "xml-rpc"
      ["version"]=>
      string(3) "1.0"
      ["name"]=>
      string(15) "package.listall"
    }
    [2]=>
    array(3) {
      ["type"]=>
      string(7) "xml-rpc"
      ["version"]=>
      string(3) "1.0"
      ["name"]=>
      string(12) "release.list"
    }
    [3]=>
    array(2) {
      ["type"]=>
      string(3) "get"
      ["version"]=>
      string(3) "1.0"
    }
  }
}
string(1065) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <provides>
  <protocol type="xml-rpc" version="1.0">package.listall</protocol>
  <protocol type="xml-rpc" version="1.0">release.list</protocol>
  <protocol type="get" version="1.0"/>
 </provides>
 <packagenameregex>[A-Z]+</packagenameregex>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"
after re-parsing
bool(true)
array(8) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["provides"]=>
  array(3) {
    [1]=>
    array(3) {
      ["type"]=>
      string(7) "xml-rpc"
      ["version"]=>
      string(3) "1.0"
      ["name"]=>
      string(15) "package.listall"
    }
    [2]=>
    array(3) {
      ["type"]=>
      string(7) "xml-rpc"
      ["version"]=>
      string(3) "1.0"
      ["name"]=>
      string(12) "release.list"
    }
    [3]=>
    array(2) {
      ["type"]=>
      string(3) "get"
      ["version"]=>
      string(3) "1.0"
    }
  }
  ["packagenameregex"]=>
  string(6) "[A-Z]+"
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
}
string(1065) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <provides>
  <protocol type="xml-rpc" version="1.0">package.listall</protocol>
  <protocol type="xml-rpc" version="1.0">release.list</protocol>
  <protocol type="get" version="1.0"/>
 </provides>
 <packagenameregex>[A-Z]+</packagenameregex>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"

test add mirror
array(9) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["provides"]=>
  array(3) {
    [1]=>
    array(3) {
      ["type"]=>
      string(7) "xml-rpc"
      ["version"]=>
      string(3) "1.0"
      ["name"]=>
      string(15) "package.listall"
    }
    [2]=>
    array(3) {
      ["type"]=>
      string(7) "xml-rpc"
      ["version"]=>
      string(3) "1.0"
      ["name"]=>
      string(12) "release.list"
    }
    [3]=>
    array(2) {
      ["type"]=>
      string(3) "get"
      ["version"]=>
      string(3) "1.0"
    }
  }
  ["packagenameregex"]=>
  string(6) "[A-Z]+"
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
  ["mirrors"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(6) "server"
      ["name"]=>
      string(14) "mirror.php.net"
      ["provides"]=>
      array(2) {
        [1]=>
        array(3) {
          ["type"]=>
          string(7) "xml-rpc"
          ["version"]=>
          string(3) "1.0"
          ["name"]=>
          string(15) "package.listall"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(7) "xml-rpc"
          ["version"]=>
          string(3) "1.0"
          ["name"]=>
          string(12) "release.list"
        }
      }
    }
  }
}
string(1313) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <provides>
  <protocol type="xml-rpc" version="1.0">package.listall</protocol>
  <protocol type="xml-rpc" version="1.0">release.list</protocol>
  <protocol type="get" version="1.0"/>
 </provides>
 <packagenameregex>[A-Z]+</packagenameregex>
 <mirrors>
  <mirror type="server" name="mirror.php.net">
   <provides>
    <protocol type="xml-rpc" version="1.0">package.listall</protocol>
    <protocol type="xml-rpc" version="1.0">release.list</protocol>
   </provides>
  </mirror>
 </mirrors>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"
after re-parsing
bool(true)
array(9) {
  ["version"]=>
  string(3) "1.0"
  ["name"]=>
  string(5) "pear5"
  ["summary"]=>
  string(23) "PEAR packages for PHP 5"
  ["server"]=>
  string(12) "pear.php.net"
  ["provides"]=>
  array(3) {
    [1]=>
    array(3) {
      ["type"]=>
      string(7) "xml-rpc"
      ["version"]=>
      string(3) "1.0"
      ["name"]=>
      string(15) "package.listall"
    }
    [2]=>
    array(3) {
      ["type"]=>
      string(7) "xml-rpc"
      ["version"]=>
      string(3) "1.0"
      ["name"]=>
      string(12) "release.list"
    }
    [3]=>
    array(2) {
      ["type"]=>
      string(3) "get"
      ["version"]=>
      string(3) "1.0"
    }
  }
  ["packagenameregex"]=>
  string(6) "[A-Z]+"
  ["mirrors"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(6) "server"
      ["name"]=>
      string(14) "mirror.php.net"
      ["provides"]=>
      array(2) {
        [1]=>
        array(3) {
          ["type"]=>
          string(7) "xml-rpc"
          ["version"]=>
          string(3) "1.0"
          ["name"]=>
          string(15) "package.listall"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(7) "xml-rpc"
          ["version"]=>
          string(3) "1.0"
          ["name"]=>
          string(12) "release.list"
        }
      }
    }
  }
  ["subchannels"]=>
  array(2) {
    [1]=>
    array(4) {
      ["name"]=>
      string(8) "pear5.qa"
      ["server"]=>
      string(15) "qa.pear.php.net"
      ["summary"]=>
      string(51) "Quality Assurance releases of unmaintained packages"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.4"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.0.0"
        }
      }
    }
    [2]=>
    array(4) {
      ["name"]=>
      string(8) "pear.boo"
      ["server"]=>
      string(16) "boo.pear.php.net"
      ["summary"]=>
      string(5) "booya"
      ["deps"]=>
      array(2) {
        [1]=>
        array(4) {
          ["type"]=>
          string(3) "pkg"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(3) "1.5"
          ["name"]=>
          string(4) "PEAR"
        }
        [2]=>
        array(3) {
          ["type"]=>
          string(3) "php"
          ["rel"]=>
          string(2) "ge"
          ["version"]=>
          string(5) "5.1.0"
        }
      }
    }
  }
  ["deps"]=>
  array(1) {
    [1]=>
    array(3) {
      ["type"]=>
      string(3) "php"
      ["rel"]=>
      string(2) "ge"
      ["version"]=>
      string(5) "5.0.0"
    }
  }
}
string(1313) "<?xml version="1.0" encoding="ISO-8859-1" ?>
<!DOCTYPE package SYSTEM "http://pear.php.net/dtd/channel-1.0">
<channel version="1.0">
 <name>pear5</name>
 <summary>PEAR packages for PHP 5</summary>
 <server>pear.php.net</server>
 <provides>
  <protocol type="xml-rpc" version="1.0">package.listall</protocol>
  <protocol type="xml-rpc" version="1.0">release.list</protocol>
  <protocol type="get" version="1.0"/>
 </provides>
 <packagenameregex>[A-Z]+</packagenameregex>
 <mirrors>
  <mirror type="server" name="mirror.php.net">
   <provides>
    <protocol type="xml-rpc" version="1.0">package.listall</protocol>
    <protocol type="xml-rpc" version="1.0">release.list</protocol>
   </provides>
  </mirror>
 </mirrors>
 <subchannels>
  <subchannel name="pear5.qa" server="qa.pear.php.net">
   <summary>Quality Assurance releases of unmaintained packages</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.4">PEAR</dep>
    <dep type="php" rel="ge" version="5.0.0"/>
   </deps>
  </subchannel>
  <subchannel name="pear.boo" server="boo.pear.php.net">
   <summary>booya</summary>
   <deps>
    <dep type="pkg" rel="ge" version="1.5">PEAR</dep>
    <dep type="php" rel="ge" version="5.1.0"/>
   </deps>
  </subchannel>
 </subchannels>
 <deps>
  <dep type="php" rel="ge" version="5.0.0"/>
 </deps>
</channel>
"