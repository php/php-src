--TEST--
Test str_replace() function for case (needle=string ; replace=array)
--INI--
precision=14
--FILE--
<?php
/* 
  Prototype: mixed str_replace(mixed $search, mixed $replace, 
                               mixed $subject [, int &$count]);
  Description: Replace all occurrences of the search string with 
               the replacement string

  This feature was requested in bug #38685: if needle is a string and replace
      is an array, each  occurence of needle is replaced by an element of the
	  replace array. If the replace array contains less elements than the
	  count of needles found, loop to the first one and continue.
	  Before this feature was implemented, when the needle was a string, the
	  replace arg was forced to a string.
*/

function check_replace2(&$result,$search,$replace,$subject,$opt_name)
{
$c=0;
$result[$opt_name]['value']=str_replace($search,$replace,$subject,$c,constant($opt_name));
$result[$opt_name]['count']=$c;
}

function check_replace($search,$replace,$subject)
{
$result=array();
check_replace2($result,$search,$replace,$subject,'STR_REPLACE_STOP');
check_replace2($result,$search,$replace,$subject,'STR_REPLACE_LOOP');
check_replace2($result,$search,$replace,$subject,'STR_REPLACE_FIRST');
check_replace2($result,$search,$replace,$subject,'STR_REPLACE_LAST');
check_replace2($result,$search,$replace,$subject,'STR_REPLACE_EMPTY');
var_dump($result);
}

//----

function msg($str)
{
echo "\n--- $str ---\n\n";
}

//----

msg("1 element");

$search='[?]';
$repl=array('alpha','beta',10, null);
$subject='Replace 1 [?] element';
check_replace($search,$repl,$subject);

msg("replace count, grouped");

$subject='Replace [?][?][?][?] repl/count grouped elements';
check_replace($search,$repl,$subject);

msg("replace count, separate");

$subject='Replace [?] repl/count [?] separate [?] elem[?]ents';
check_replace($search,$repl,$subject);

msg("multiple, grouped");

$subject='Replace [?][?][?][?][?][?][?][?][?] grouped elements';
check_replace($search,$repl,$subject);

msg("multiple, separate");

$subject='[?]Rep[?]la[?]ce [?] ma[?]ny[?] [?] s[?]epa[?]rate [?]';
check_replace($search,$repl,$subject);

msg("Empty replace array");

$subject='[?]Rep[?]la[?]ce[?]';
$repl=array();
check_replace($search,$repl,$subject);

msg("replace elements contain needle");

$subject='[?]Rep[?]la[?]ce[?] [?]ma[?]ny[?] ';
$repl=array('al[?]pz[?]',"[?]b[?]v\n",null,37,"[?]\n[?]");
check_replace($search,$repl,$subject);

msg("subject == needle");

$search='FOOOOOOOOOOOOOOOOOOOOOOOOOO';
$subject=$search;
$repl=array(1,2,3);
check_replace($search,$repl,$subject);

msg("length(subject) < length(needle)");

$subject='FOOOOOOOOOOOOOOOOOOOOOOOOOO';
$search=$subject."\n";
$repl=array(1,2,3);
check_replace($search,$repl,$subject);

msg("Multi-level array");

$subject='[?](?)Rep[?]la[?(?)]ce [?]ma';
$repl=array(
	array('al[?]p(?)z[?]',"[?]b(?)v\n",null,37,"\n[?]"),
	array('zk(?)j[?]', null, 'ab(?)c', 45)
	);
$search=array('[?]','(?)');
check_replace($search,$repl,$subject);

msg("Multi-level - empty array");

$subject='[?](?)Rep[?]la[?](?)c<a>e';
$repl=array(
	array('ap(?)z[?]',"b[?](?)v\n",null,37,"[?]\n[?]"),
	array(),
	array('k(?)j[?]', null, 'ab(?)c', 45)
	);
$search=array('[?]','(?)','<a>');
check_replace($search,$repl,$subject);

msg("Multi-level - deep 1");

$search=array('[?]',array('(?)','d','e'),'a','R');
$repl=array(
	array('ap(?)z[?]',"b[?](?)v",null,37,"[?]n[?]"),
	array('a',array('b',null)),
	array('k(?)j[?]')
	);
$subject=array('[?](?)Re[?](?)c<a>e'
	,'mykey' => array('[?]dabdee(?)ab[?][?]dd(?)ec(?)'
		,array(null, 'key2' => $search, $repl, 10, 20)
		),
	$repl);
check_replace($search,$repl,$subject);

msg('Invalid options value');

$c=0;
var_dump(str_replace('a','b','abc',$c,50));
var_dump($c);

?>
===DONE===
--EXPECTF--	
--- 1 element ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(23) "Replace 1 alpha element"
    ["count"]=>
    int(1)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(23) "Replace 1 alpha element"
    ["count"]=>
    int(1)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(23) "Replace 1 alpha element"
    ["count"]=>
    int(1)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(23) "Replace 1 alpha element"
    ["count"]=>
    int(1)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(23) "Replace 1 alpha element"
    ["count"]=>
    int(1)
  }
}

--- replace count, grouped ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(47) "Replace alphabeta10 repl/count grouped elements"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(47) "Replace alphabeta10 repl/count grouped elements"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(47) "Replace alphabeta10 repl/count grouped elements"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(47) "Replace alphabeta10 repl/count grouped elements"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(47) "Replace alphabeta10 repl/count grouped elements"
    ["count"]=>
    int(4)
  }
}

--- replace count, separate ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(50) "Replace alpha repl/count beta separate 10 elements"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(50) "Replace alpha repl/count beta separate 10 elements"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(50) "Replace alpha repl/count beta separate 10 elements"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(50) "Replace alpha repl/count beta separate 10 elements"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(50) "Replace alpha repl/count beta separate 10 elements"
    ["count"]=>
    int(4)
  }
}

--- multiple, grouped ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(51) "Replace alphabeta10[?][?][?][?][?] grouped elements"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(52) "Replace alphabeta10alphabeta10alpha grouped elements"
    ["count"]=>
    int(9)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(61) "Replace alphabeta10alphaalphaalphaalphaalpha grouped elements"
    ["count"]=>
    int(9)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(36) "Replace alphabeta10 grouped elements"
    ["count"]=>
    int(9)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(36) "Replace alphabeta10 grouped elements"
    ["count"]=>
    int(9)
  }
}

--- multiple, separate ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(53) "alphaRepbetala10ce  ma[?]ny[?] [?] s[?]epa[?]rate [?]"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(55) "alphaRepbetala10ce  maalphanybeta 10 sepaalpharate beta"
    ["count"]=>
    int(10)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(65) "alphaRepbetala10ce  maalphanyalpha alpha salphaepaalpharate alpha"
    ["count"]=>
    int(10)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(35) "alphaRepbetala10ce  many  separate "
    ["count"]=>
    int(10)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(35) "alphaRepbetala10ce  many  separate "
    ["count"]=>
    int(10)
  }
}

--- Empty replace array ---


Warning: str_replace(): Replace array must not be empty in %s on line %d

Warning: str_replace(): Replace array must not be empty in %s on line %d

Warning: str_replace(): Replace array must not be empty in %s on line %d

Warning: str_replace(): Replace array must not be empty in %s on line %d

Warning: str_replace(): Replace array must not be empty in %s on line %d
array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(19) "[?]Rep[?]la[?]ce[?]"
    ["count"]=>
    int(0)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(19) "[?]Rep[?]la[?]ce[?]"
    ["count"]=>
    int(0)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(19) "[?]Rep[?]la[?]ce[?]"
    ["count"]=>
    int(0)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(19) "[?]Rep[?]la[?]ce[?]"
    ["count"]=>
    int(0)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(19) "[?]Rep[?]la[?]ce[?]"
    ["count"]=>
    int(0)
  }
}

--- replace elements contain needle ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(47) "al[?]pz[?]Rep[?]b[?]v
lace37 [?]
[?]ma[?]ny[?] "
    ["count"]=>
    int(5)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(60) "al[?]pz[?]Rep[?]b[?]v
lace37 [?]
[?]maal[?]pz[?]ny[?]b[?]v
 "
    ["count"]=>
    int(7)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(61) "al[?]pz[?]Rep[?]b[?]v
lace37 [?]
[?]maal[?]pz[?]nyal[?]pz[?] "
    ["count"]=>
    int(7)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(55) "al[?]pz[?]Rep[?]b[?]v
lace37 [?]
[?]ma[?]
[?]ny[?]
[?] "
    ["count"]=>
    int(7)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(41) "al[?]pz[?]Rep[?]b[?]v
lace37 [?]
[?]many "
    ["count"]=>
    int(7)
  }
}

--- subject == needle ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(1) "1"
    ["count"]=>
    int(1)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(1) "1"
    ["count"]=>
    int(1)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(1) "1"
    ["count"]=>
    int(1)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(1) "1"
    ["count"]=>
    int(1)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(1) "1"
    ["count"]=>
    int(1)
  }
}

--- length(subject) < length(needle) ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(27) "FOOOOOOOOOOOOOOOOOOOOOOOOOO"
    ["count"]=>
    int(0)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(27) "FOOOOOOOOOOOOOOOOOOOOOOOOOO"
    ["count"]=>
    int(0)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(27) "FOOOOOOOOOOOOOOOOOOOOOOOOOO"
    ["count"]=>
    int(0)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(27) "FOOOOOOOOOOOOOOOOOOOOOOOOOO"
    ["count"]=>
    int(0)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(27) "FOOOOOOOOOOOOOOOOOOOOOOOOOO"
    ["count"]=>
    int(0)
  }
}

--- Multi-level array ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(46) "al[?]pzk(?)j[?]z[?]Rep[?]bab(?)cv
la[?45]ce ma"
    ["count"]=>
    int(7)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(46) "al[?]pzk(?)j[?]z[?]Rep[?]bab(?)cv
la[?45]ce ma"
    ["count"]=>
    int(7)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(46) "al[?]pzk(?)j[?]z[?]Rep[?]bab(?)cv
la[?45]ce ma"
    ["count"]=>
    int(7)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(46) "al[?]pzk(?)j[?]z[?]Rep[?]bab(?)cv
la[?45]ce ma"
    ["count"]=>
    int(7)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(46) "al[?]pzk(?)j[?]z[?]Rep[?]bab(?)cv
la[?45]ce ma"
    ["count"]=>
    int(7)
  }
}

--- Multi-level - empty array ---


Warning: str_replace(): Replace array must not be empty in %s on line 19

Warning: str_replace(): Replace array must not be empty in %s on line 19

Warning: str_replace(): Replace array must not be empty in %s on line 19

Warning: str_replace(): Replace array must not be empty in %s on line 19

Warning: str_replace(): Replace array must not be empty in %s on line 19
array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    string(39) "ap(?)z[?](?)Repb[?](?)v
la(?)ck(?)j[?]e"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    string(39) "ap(?)z[?](?)Repb[?](?)v
la(?)ck(?)j[?]e"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    string(39) "ap(?)z[?](?)Repb[?](?)v
la(?)ck(?)j[?]e"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    string(39) "ap(?)z[?](?)Repb[?](?)v
la(?)ck(?)j[?]e"
    ["count"]=>
    int(4)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    string(39) "ap(?)z[?](?)Repb[?](?)v
la(?)ck(?)j[?]e"
    ["count"]=>
    int(4)
  }
}

--- Multi-level - deep 1 ---

array(5) {
  ["STR_REPLACE_STOP"]=>
  array(2) {
    ["value"]=>
    array(3) {
      [0]=>
      string(26) "k(?)j[?]paz[?]ab[?]avac<a>"
      ["mykey"]=>
      array(2) {
        [0]=>
        string(31) "k(?)j[?]paz[?]babaabb[?]avddaca"
        [1]=>
        array(5) {
          [0]=>
          string(0) ""
          ["key2"]=>
          array(4) {
            [0]=>
            string(14) "k(?)j[?]paz[?]"
            [1]=>
            array(3) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              string(1) "b"
              [2]=>
              string(0) ""
            }
            [2]=>
            string(8) "k(?)j[?]"
            [3]=>
            string(0) ""
          }
          [1]=>
          array(3) {
            [0]=>
            array(5) {
              [0]=>
              string(18) "k(?)j[?]pazapaz[?]"
              [1]=>
              string(17) "bk(?)j[?]paz[?]av"
              [2]=>
              string(0) ""
              [3]=>
              string(2) "37"
              [4]=>
              string(21) "k(?)j[?]paz[?]nb[?]av"
            }
            [1]=>
            array(2) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              array(2) {
                [0]=>
                string(1) "b"
                [1]=>
                string(0) ""
              }
            }
            [2]=>
            array(1) {
              [0]=>
              string(17) "kk(?)j[?]japaz[?]"
            }
          }
          [2]=>
          string(2) "10"
          [3]=>
          string(2) "20"
        }
      }
      [1]=>
      array(3) {
        [0]=>
        array(5) {
          [0]=>
          string(18) "k(?)j[?]pazapaz[?]"
          [1]=>
          string(17) "bk(?)j[?]paz[?]av"
          [2]=>
          string(0) ""
          [3]=>
          string(2) "37"
          [4]=>
          string(21) "k(?)j[?]paz[?]nb[?]av"
        }
        [1]=>
        array(2) {
          [0]=>
          string(8) "k(?)j[?]"
          [1]=>
          array(2) {
            [0]=>
            string(1) "b"
            [1]=>
            string(0) ""
          }
        }
        [2]=>
        array(1) {
          [0]=>
          string(17) "kk(?)j[?]japaz[?]"
        }
      }
    }
    ["count"]=>
    int(69)
  }
  ["STR_REPLACE_LOOP"]=>
  array(2) {
    ["value"]=>
    array(3) {
      [0]=>
      string(61) "k(?)j[?]pk(?)j[?]z[?]k(?)j[?]b[?]k(?)j[?]vk(?)j[?]c<k(?)j[?]>"
      ["mykey"]=>
      array(2) {
        [0]=>
        string(79) "k(?)j[?]pk(?)j[?]z[?]bk(?)j[?]bk(?)j[?]k(?)j[?]bb[?]k(?)j[?]vbk(?)j[?]ck(?)j[?]"
        [1]=>
        array(5) {
          [0]=>
          string(0) ""
          ["key2"]=>
          array(4) {
            [0]=>
            string(21) "k(?)j[?]pk(?)j[?]z[?]"
            [1]=>
            array(3) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              string(1) "b"
              [2]=>
              string(0) ""
            }
            [2]=>
            string(8) "k(?)j[?]"
            [3]=>
            string(0) ""
          }
          [1]=>
          array(3) {
            [0]=>
            array(5) {
              [0]=>
              string(39) "k(?)j[?]pk(?)j[?]zk(?)j[?]pk(?)j[?]z[?]"
              [1]=>
              string(31) "bk(?)j[?]pk(?)j[?]z[?]k(?)j[?]v"
              [2]=>
              string(0) ""
              [3]=>
              string(2) "37"
              [4]=>
              string(35) "k(?)j[?]pk(?)j[?]z[?]nb[?]k(?)j[?]v"
            }
            [1]=>
            array(2) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              array(2) {
                [0]=>
                string(1) "b"
                [1]=>
                string(0) ""
              }
            }
            [2]=>
            array(1) {
              [0]=>
              string(31) "kk(?)j[?]jk(?)j[?]pk(?)j[?]z[?]"
            }
          }
          [2]=>
          string(2) "10"
          [3]=>
          string(2) "20"
        }
      }
      [1]=>
      array(3) {
        [0]=>
        array(5) {
          [0]=>
          string(39) "k(?)j[?]pk(?)j[?]zk(?)j[?]pk(?)j[?]z[?]"
          [1]=>
          string(31) "bk(?)j[?]pk(?)j[?]z[?]k(?)j[?]v"
          [2]=>
          string(0) ""
          [3]=>
          string(2) "37"
          [4]=>
          string(35) "k(?)j[?]pk(?)j[?]z[?]nb[?]k(?)j[?]v"
        }
        [1]=>
        array(2) {
          [0]=>
          string(8) "k(?)j[?]"
          [1]=>
          array(2) {
            [0]=>
            string(1) "b"
            [1]=>
            string(0) ""
          }
        }
        [2]=>
        array(1) {
          [0]=>
          string(31) "kk(?)j[?]jk(?)j[?]pk(?)j[?]z[?]"
        }
      }
    }
    ["count"]=>
    int(102)
  }
  ["STR_REPLACE_FIRST"]=>
  array(2) {
    ["value"]=>
    array(3) {
      [0]=>
      string(61) "k(?)j[?]pk(?)j[?]z[?]k(?)j[?]b[?]k(?)j[?]vk(?)j[?]c<k(?)j[?]>"
      ["mykey"]=>
      array(2) {
        [0]=>
        string(80) "k(?)j[?]pk(?)j[?]z[?]bk(?)j[?]bk(?)j[?]k(?)j[?]bb[?]k(?)j[?]vbbk(?)j[?]ck(?)j[?]"
        [1]=>
        array(5) {
          [0]=>
          string(0) ""
          ["key2"]=>
          array(4) {
            [0]=>
            string(21) "k(?)j[?]pk(?)j[?]z[?]"
            [1]=>
            array(3) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              string(1) "b"
              [2]=>
              string(0) ""
            }
            [2]=>
            string(8) "k(?)j[?]"
            [3]=>
            string(0) ""
          }
          [1]=>
          array(3) {
            [0]=>
            array(5) {
              [0]=>
              string(39) "k(?)j[?]pk(?)j[?]zk(?)j[?]pk(?)j[?]z[?]"
              [1]=>
              string(31) "bk(?)j[?]pk(?)j[?]z[?]k(?)j[?]v"
              [2]=>
              string(0) ""
              [3]=>
              string(2) "37"
              [4]=>
              string(35) "k(?)j[?]pk(?)j[?]z[?]nb[?]k(?)j[?]v"
            }
            [1]=>
            array(2) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              array(2) {
                [0]=>
                string(1) "b"
                [1]=>
                string(0) ""
              }
            }
            [2]=>
            array(1) {
              [0]=>
              string(31) "kk(?)j[?]jk(?)j[?]pk(?)j[?]z[?]"
            }
          }
          [2]=>
          string(2) "10"
          [3]=>
          string(2) "20"
        }
      }
      [1]=>
      array(3) {
        [0]=>
        array(5) {
          [0]=>
          string(39) "k(?)j[?]pk(?)j[?]zk(?)j[?]pk(?)j[?]z[?]"
          [1]=>
          string(31) "bk(?)j[?]pk(?)j[?]z[?]k(?)j[?]v"
          [2]=>
          string(0) ""
          [3]=>
          string(2) "37"
          [4]=>
          string(35) "k(?)j[?]pk(?)j[?]z[?]nb[?]k(?)j[?]v"
        }
        [1]=>
        array(2) {
          [0]=>
          string(8) "k(?)j[?]"
          [1]=>
          array(2) {
            [0]=>
            string(1) "b"
            [1]=>
            string(0) ""
          }
        }
        [2]=>
        array(1) {
          [0]=>
          string(31) "kk(?)j[?]jk(?)j[?]pk(?)j[?]z[?]"
        }
      }
    }
    ["count"]=>
    int(102)
  }
  ["STR_REPLACE_LAST"]=>
  array(2) {
    ["value"]=>
    array(3) {
      [0]=>
      string(61) "k(?)j[?]pk(?)j[?]z[?]k(?)j[?]b[?]k(?)j[?]vk(?)j[?]c<k(?)j[?]>"
      ["mykey"]=>
      array(2) {
        [0]=>
        string(78) "k(?)j[?]pk(?)j[?]z[?]bk(?)j[?]bk(?)j[?]k(?)j[?]bb[?]k(?)j[?]vk(?)j[?]ck(?)j[?]"
        [1]=>
        array(5) {
          [0]=>
          string(0) ""
          ["key2"]=>
          array(4) {
            [0]=>
            string(21) "k(?)j[?]pk(?)j[?]z[?]"
            [1]=>
            array(3) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              string(1) "b"
              [2]=>
              string(0) ""
            }
            [2]=>
            string(8) "k(?)j[?]"
            [3]=>
            string(0) ""
          }
          [1]=>
          array(3) {
            [0]=>
            array(5) {
              [0]=>
              string(39) "k(?)j[?]pk(?)j[?]zk(?)j[?]pk(?)j[?]z[?]"
              [1]=>
              string(31) "bk(?)j[?]pk(?)j[?]z[?]k(?)j[?]v"
              [2]=>
              string(0) ""
              [3]=>
              string(2) "37"
              [4]=>
              string(35) "k(?)j[?]pk(?)j[?]z[?]nb[?]k(?)j[?]v"
            }
            [1]=>
            array(2) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              array(2) {
                [0]=>
                string(1) "b"
                [1]=>
                string(0) ""
              }
            }
            [2]=>
            array(1) {
              [0]=>
              string(31) "kk(?)j[?]jk(?)j[?]pk(?)j[?]z[?]"
            }
          }
          [2]=>
          string(2) "10"
          [3]=>
          string(2) "20"
        }
      }
      [1]=>
      array(3) {
        [0]=>
        array(5) {
          [0]=>
          string(39) "k(?)j[?]pk(?)j[?]zk(?)j[?]pk(?)j[?]z[?]"
          [1]=>
          string(31) "bk(?)j[?]pk(?)j[?]z[?]k(?)j[?]v"
          [2]=>
          string(0) ""
          [3]=>
          string(2) "37"
          [4]=>
          string(35) "k(?)j[?]pk(?)j[?]z[?]nb[?]k(?)j[?]v"
        }
        [1]=>
        array(2) {
          [0]=>
          string(8) "k(?)j[?]"
          [1]=>
          array(2) {
            [0]=>
            string(1) "b"
            [1]=>
            string(0) ""
          }
        }
        [2]=>
        array(1) {
          [0]=>
          string(31) "kk(?)j[?]jk(?)j[?]pk(?)j[?]z[?]"
        }
      }
    }
    ["count"]=>
    int(102)
  }
  ["STR_REPLACE_EMPTY"]=>
  array(2) {
    ["value"]=>
    array(3) {
      [0]=>
      string(21) "k(?)j[?]pz[?]b[?]vc<>"
      ["mykey"]=>
      array(2) {
        [0]=>
        string(22) "k(?)j[?]pz[?]bbbb[?]vc"
        [1]=>
        array(5) {
          [0]=>
          string(0) ""
          ["key2"]=>
          array(4) {
            [0]=>
            string(13) "k(?)j[?]pz[?]"
            [1]=>
            array(3) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              string(1) "b"
              [2]=>
              string(0) ""
            }
            [2]=>
            string(8) "k(?)j[?]"
            [3]=>
            string(0) ""
          }
          [1]=>
          array(3) {
            [0]=>
            array(5) {
              [0]=>
              string(15) "k(?)j[?]pzpz[?]"
              [1]=>
              string(15) "bk(?)j[?]pz[?]v"
              [2]=>
              string(0) ""
              [3]=>
              string(2) "37"
              [4]=>
              string(19) "k(?)j[?]pz[?]nb[?]v"
            }
            [1]=>
            array(2) {
              [0]=>
              string(8) "k(?)j[?]"
              [1]=>
              array(2) {
                [0]=>
                string(1) "b"
                [1]=>
                string(0) ""
              }
            }
            [2]=>
            array(1) {
              [0]=>
              string(15) "kk(?)j[?]jpz[?]"
            }
          }
          [2]=>
          string(2) "10"
          [3]=>
          string(2) "20"
        }
      }
      [1]=>
      array(3) {
        [0]=>
        array(5) {
          [0]=>
          string(15) "k(?)j[?]pzpz[?]"
          [1]=>
          string(15) "bk(?)j[?]pz[?]v"
          [2]=>
          string(0) ""
          [3]=>
          string(2) "37"
          [4]=>
          string(19) "k(?)j[?]pz[?]nb[?]v"
        }
        [1]=>
        array(2) {
          [0]=>
          string(8) "k(?)j[?]"
          [1]=>
          array(2) {
            [0]=>
            string(1) "b"
            [1]=>
            string(0) ""
          }
        }
        [2]=>
        array(1) {
          [0]=>
          string(15) "kk(?)j[?]jpz[?]"
        }
      }
    }
    ["count"]=>
    int(102)
  }
}

--- Invalid options value ---


Warning: str_replace(): Invalid options value (50) in %s on line %d
string(3) "bbc"
int(1)
===DONE===
